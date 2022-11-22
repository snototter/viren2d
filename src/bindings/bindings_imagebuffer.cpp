#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <limits>

#include <pybind11/operators.h>
#include <pybind11/numpy.h>

#include <bindings/binding_helpers.h>
#include <helpers/logging.h>
#include <viren2d/imagebuffer.h>


namespace py = pybind11;

namespace viren2d {
namespace bindings {
//------------------------------------------------- ImageBuffer from numpy array
//FIXME extend imagebuffer conversion by:
  // 1) enable creation from int8, bool (also provide bool.convert_to_uchar)
  // 2) DONE non-c-contig --> iterate pixel by pixel

/// Returns the ImageBufferType for the array's dtype.
/// As this method is only used during NumPy --> C++ conversion, a
/// ValueError (std::invalid_argument) will be raised (thrown) for
/// unsupported dtypes.
inline ImageBufferType ImageBufferTypeFromPyArray(const py::array &arr) {
  if (py::isinstance<py::array_t<uint8_t>>(arr)) {
    return ImageBufferType::UInt8;
  } else if (py::isinstance<py::array_t<int16_t>>(arr)) {
    return ImageBufferType::Int16;
  } else if (py::isinstance<py::array_t<uint16_t>>(arr)) {
    return ImageBufferType::UInt16;
  } else if (py::isinstance<py::array_t<int32_t>>(arr)) {
    return ImageBufferType::Int32;
  } else if (py::isinstance<py::array_t<uint32_t>>(arr)) {
    return ImageBufferType::UInt32;
  } else if (py::isinstance<py::array_t<int64_t>>(arr)) {
    return ImageBufferType::Int64;
  } else if (py::isinstance<py::array_t<uint64_t>>(arr)) {
    return ImageBufferType::UInt64;
  } else if (py::isinstance<py::array_t<float>>(arr)) {
    return ImageBufferType::Float;
  } else if (py::isinstance<py::array_t<double>>(arr)) {
    return ImageBufferType::Double;
  } else {
    const py::dtype dtype = arr.dtype();
    std::string s("Incompatible `dtype` (");
    s += py::cast<std::string>(dtype.attr("name"));
    s += ", \"";
    const py::list dt_descr = py::cast<py::list>(dtype.attr("descr"));
    for (std::size_t i = 0; i < dt_descr.size(); ++i) {
      // First element holds the optional name, second one holds the
      // type description we're interested in, check for example:
      // https://numpy.org/doc/stable/reference/generated/numpy.dtype.descr.html
      const py::tuple td = py::cast<py::tuple>(dt_descr[i]);
      s += py::cast<std::string>(td[1]);
      if (i < dt_descr.size() - 1) {
        s += "\", \"";
      }
    }
    s += "\") to compute the ImageBufferType!";
    SPDLOG_ERROR(s);
    throw std::invalid_argument(s);
  }
}


ImageBuffer CreateImageBuffer(
    py::array &buf, bool copy, bool disable_warnings) {
  // Sanity checks
  if (buf.ndim() < 2 || buf.ndim() > 3) {
    std::ostringstream s;
    s << "Incompatible buffer dimensions - "
         "expected `ndim` to be 2 or 3, but got: "
      << buf.ndim() << '!';
    SPDLOG_ERROR(s.str());
    throw std::invalid_argument(s.str());
  }


  const ImageBufferType buffer_type = ImageBufferTypeFromPyArray(buf);

  if (ElementSizeFromImageBufferType(buffer_type) != static_cast<int>(buf.itemsize())) {
    std::ostringstream s;
    s << "ImageBuffer `" << ImageBufferTypeToString(buffer_type)
      << "` expected item size " << ElementSizeFromImageBufferType(buffer_type)
      << " bytes, but python buffer info states " << buf.itemsize() << "!";
    SPDLOG_ERROR(s.str());
    throw std::logic_error(s.str());
  }


  ImageBuffer img;
  const int row_stride = static_cast<int>(buf.strides(0));
  const int col_stride = static_cast<int>(buf.strides(1));
  const int channel_stride = (buf.ndim() == 2) ? 1 : static_cast<int>(buf.strides(2));
  const int height = static_cast<int>(buf.shape(0));
  const int width = static_cast<int>(buf.shape(1));
  const int channels = (buf.ndim() == 2) ? 1 : static_cast<int>(buf.shape(2));

  //TODO refactor/clean up --> check/set copy flag; move createcopy/share outside of if/else
  if ((buf.flags() & py::array::c_style) != py::array::c_style) {
    // Non-contiguous buffers must be copied element-wise:
    if (!copy && !disable_warnings) {
      SPDLOG_WARN(
            "Input python array is not row-major. The "
            "`viren2d.ImageBuffer` will be created as a copy, which ignores "
            "the input parameter `copy=False`.");
    }
    img.CreateCopiedBuffer(
          static_cast<unsigned char const*>(buf.data()),
          height, width, channels,
          row_stride, col_stride, channel_stride,
          buffer_type);
  } else {
    // C-style buffers can be shared & easily copied.

    // Sharing, however, requires that the buffer is mutable.
    // To prevent exceptions, we instead log a warning and fall back to
    // copying the data: viren2d is a visualization toolbox, I prefer
    // some overhead (memory copy) over exceptions during python/cpp
    // type conversion.
    if (!copy && !buf.writeable()) {
      if (!disable_warnings) {
        SPDLOG_WARN(
              "Input python array is not writeable. The "
              "`viren2d.ImageBuffer` will be created as a copy, which ignores "
              "the input parameter `copy=False`.");
      }
      copy = true;
    }

    // If the stride is negative (e.g. when casting `image[:,:,::-1]`),
    // we need to force a deep copy.
    if (!copy && ((row_stride < 0) || (col_stride < 0) || (channel_stride < 0))) {
      if (!disable_warnings) {
        SPDLOG_WARN(
              "Channel stride ({:d}) of python array does not match "
              "itemsize ({:d}). The `viren2d.ImageBuffer` will be created as a "
              "copy, which ignores the input parameter `copy=False`.",
              channel_stride, static_cast<int>(buf.itemsize()));
      }
      copy = true;
    }

    if (copy) {
      img.CreateCopiedBuffer(
            static_cast<unsigned char const*>(buf.data()),
            height, width, channels,
            row_stride, col_stride, channel_stride, buffer_type);
    } else {
      img.CreateSharedBuffer(
            static_cast<unsigned char*>(buf.mutable_data()),
            height, width, channels, row_stride, col_stride, buffer_type);
    }
  }
  return img;
}


//FIXME if input is contiguous, use memcpy!
template<typename _T>
ImageBuffer ConvertBufferToUInt8C4Helper(const py::array &buf, _T scale) {
  const int row_stride = static_cast<int>(buf.strides(0));
  const int col_stride = static_cast<int>(buf.strides(1));
  const int channel_stride = (buf.ndim() == 2) ? 1 : static_cast<int>(buf.strides(2));
  const int height = static_cast<int>(buf.shape(0));
  const int width = static_cast<int>(buf.shape(1));
  const int channels = (buf.ndim() == 2) ? 1 : static_cast<int>(buf.shape(2));

  ImageBuffer img8u4(height, width, 4, ImageBufferType::UInt8);
  const uint8_t* src_data = static_cast<const uint8_t*>(buf.data());
  //TODO check: https://pybind11.readthedocs.io/en/stable/advanced/pycpp/numpy.html#direct-access

  // Copy pixel by pixel because the input buffer might be a sliced,
  // transposed, or any other view (e.g. with negative strides)
  for (int row = 0, src_row_offset = 0;
       row < height;
       ++row, src_row_offset += row_stride) {
    // The destination buffer is freshly allocated, thus the memory
    // is aligned.
    uint8_t *dst_ptr = img8u4.MutablePtr<uint8_t>(row, 0, 0);

    for (int col = 0, src_col_offset = 0;
         col < width;
         ++col, src_col_offset += col_stride) {

      for (int ch = 0, src_channel_offset = 0;
           ch < 4;
           ++ch, src_channel_offset += channel_stride) {
        if (ch < channels) {
          // Strides in python buffers are in bytes, thus we need
          // to cast the resulting address to the proper type.
          const _T* src_ptr = reinterpret_cast<const _T*>(
                src_data + src_row_offset + src_col_offset + src_channel_offset);
          *dst_ptr = static_cast<uint8_t>((*src_ptr) * scale);
        } else {
          if (ch == 3) {
            *dst_ptr = 255;
          } else {
            *dst_ptr = img8u4.AtUnchecked<uint8_t>(row, col, 0);
          }
        }
        ++dst_ptr;
      }
    }
  }

  return img8u4;
}


ImageBuffer CastToImageBufferUInt8C4(py::array buf) {
  // Sanity checks
  if (buf.ndim() < 2 || buf.ndim() > 3) {
    std::ostringstream s;
    s << "Incompatible buffer dimensions - "
         "expected `ndim` to be 2 or 3, but got: "
      << buf.ndim() << '!';
    SPDLOG_ERROR(s.str());
    throw std::invalid_argument(s.str());
  }

  const ImageBufferType buffer_type = ImageBufferTypeFromPyArray(buf);
  if (ElementSizeFromImageBufferType(buffer_type) != static_cast<int>(buf.itemsize())) {
    std::ostringstream s;
    s << "ImageBuffer `" << ImageBufferTypeToString(buffer_type)
      << "` expected item size " << ElementSizeFromImageBufferType(buffer_type)
      << " bytes, but python buffer info states " << buf.itemsize() << "!";
    SPDLOG_ERROR(s.str());
    throw std::logic_error(s.str());
  }

  const int channels = (buf.ndim() == 2) ? 1 : static_cast<int>(buf.shape(2));
  if ((channels != 1) && (channels != 3) && (channels != 4)) {
    std::ostringstream s;
    s << "Python buffer has " << channels
      << " channels, but only 1, 3, or 4 are supported for conversion to "
         "a 4-channel uint8 ImageBuffer!";
    SPDLOG_ERROR(s.str());
    throw std::logic_error(s.str());
  }

  switch (buffer_type) {
    case ImageBufferType::UInt8: {
        if (channels == 4) {
          return CreateImageBuffer(buf, false, true);
        } else {
          return ConvertBufferToUInt8C4Helper<uint8_t>(buf, 1);
        }
      }

    case ImageBufferType::Int16:
      return ConvertBufferToUInt8C4Helper<int16_t>(buf, 1);

    case ImageBufferType::UInt16:
      return ConvertBufferToUInt8C4Helper<int16_t>(buf, 1);

    case ImageBufferType::Int32:
      return ConvertBufferToUInt8C4Helper<int32_t>(buf, 1);

    case ImageBufferType::UInt32:
      return ConvertBufferToUInt8C4Helper<uint32_t>(buf, 1);

    case ImageBufferType::Int64:
      return ConvertBufferToUInt8C4Helper<int64_t>(buf, 1);

    case ImageBufferType::UInt64:
      return ConvertBufferToUInt8C4Helper<uint64_t>(buf, 1);

    case ImageBufferType::Float:
      return ConvertBufferToUInt8C4Helper<float>(buf, 255);

    case ImageBufferType::Double:
      return ConvertBufferToUInt8C4Helper<double>(buf, 255);
  }

  std::string s("Conversion from python array of type `");
  s += ImageBufferTypeToString(buffer_type);
  s += " ` is not yet supported!";
  SPDLOG_ERROR(s);
  throw std::logic_error(s);
}


inline std::string FormatDescriptor(ImageBufferType t) {
  switch (t) {
    case ImageBufferType::UInt8:
      return py::format_descriptor<uint8_t>::format();

    case ImageBufferType::Int16:
      return py::format_descriptor<int16_t>::format();

    case ImageBufferType::UInt16:
      return py::format_descriptor<uint16_t>::format();

    case ImageBufferType::Int32:
      return py::format_descriptor<int32_t>::format();

    case ImageBufferType::UInt32:
      return py::format_descriptor<uint32_t>::format();

    case ImageBufferType::Int64:
      return py::format_descriptor<int64_t>::format();

    case ImageBufferType::UInt64:
      return py::format_descriptor<uint64_t>::format();

    case ImageBufferType::Float:
      return py::format_descriptor<float>::format();

    case ImageBufferType::Double:
      return py::format_descriptor<double>::format();
  }

  std::string s("ImageBufferType `");
  s += ImageBufferTypeToString(t);
  s += "` not handled in `FormatDescriptor` switch!";
  SPDLOG_ERROR(s);
  throw std::logic_error(s);
}


py::buffer_info ImageBufferInfo(ImageBuffer &img) {
  return py::buffer_info(
      img.MutableData(),
      static_cast<std::size_t>(
          (img.ElementSize() > 0)
          ? img.ElementSize()
          : ElementSizeFromImageBufferType(img.BufferType())), // Size of each element
      FormatDescriptor(img.BufferType()), // Python struct-style format descriptor
      3,  //Always return ndim=3 (by design)
      { static_cast<std::size_t>(img.Height()),
        static_cast<std::size_t>(img.Width()),
        static_cast<std::size_t>(img.Channels()) }, // Buffer dimensions
      { static_cast<std::size_t>(img.RowStride()),
        static_cast<std::size_t>(img.PixelStride()),
        static_cast<std::size_t>(img.ElementSize()) } // Strides (in bytes) per dimension
  );
}


std::string PathStringFromPyObject(const py::object &path) {
  if (py::isinstance<py::str>(path)) {
    return path.cast<std::string>();
  } else {
    return py::str(path).cast<std::string>();
  }
}


ImageBuffer LoadImageUInt8Helper(
    const py::object &path, int force_num_channels) {
  return LoadImageUInt8(PathStringFromPyObject(path), force_num_channels);
}


void SaveImageUInt8Helper(
    const py::object &path, const ImageBuffer &image) {
  SaveImageUInt8(PathStringFromPyObject(path), image);
}


void RegisterImageBuffer(py::module &m) {
  py::class_<ImageBuffer> imgbuf(m, "ImageBuffer", py::buffer_protocol(), R"docstr(
        Encapsulates image data.

        This class is primarily used to pass images between the client
        application and ``viren2d``. Supported data types are:
        :class:`numpy.uint8`, :class:`numpy.int16`, :class:`numpy.uint16`,
        :class:`numpy.int32`, :class:`numpy.uint32`, :class:`numpy.int64`,
        :class:`numpy.uint64`, :class:`numpy.float32`, and :class:`numpy.float64`.
        Additionally, it provides several basic image manipulation methods
        to adjust an image quickly for visualization.

        The *ImageBuffer* implements the standard Python buffer protocol
        and can thus be swiftly converted to/from other buffer types,
        such as a :class:`numpy.ndarray`, for example:

        >>> # Create a shared ImageBuffer from a numpy.ndarray
        >>> img_buf = viren2d.ImageBuffer(img_np, copy=False)

        >>> # Create a shared numpy.ndarray from an ImageBuffer
        >>> img_np = np.array(img_buf, copy=False)

        Important:
           A :class:`numpy.ndarray` can be implicitly converted to an
           :class:`~viren2d.ImageBuffer`. Thus, there is no need for explicit
           conversion when calling a ``viren2d`` function which expects an
           :class:`~viren2d.ImageBuffer`.

           Note that by default, ``viren2d`` tries to create a **shared buffer**.
           However, if the input :class:`numpy.ndarray` is **not row-major**,
           the implicit conversion will **always create a copy**.  This would
           result in a warning message.
           To avoid this warning, either explicitly create
           a copy via ``viren2d.ImageBuffer(non_row_major_array, copy=True)``,
           or disable the warning via the optional parameter
           ``viren2d.ImageBuffer(non_row_major_array, disable_warnings=True)``.
           Alternatively, the input :class:`numpy.ndarray` could be converted
           to a C-style before invoking any ``viren2d`` function, *e.g.*
           via :func:`numpy.ascontiguousarray`.
        )docstr");

  imgbuf.def(
        py::init(&CreateImageBuffer), R"docstr(
        Creates an *ImageBuffer* from a :class:`numpy.ndarray`.

        Note:
          If the provided array is "incompatible" with the ImageBuffer
          implementation (*e.g.* requesting a shared buffer but the
          array is not mutable, or the array view has negative strides,
          etc.), the ImageBuffer will enforce a deep copy (as this
          results in a contiguous, row-major buffer).
          If this overrides the ``copy`` parameter, a warning message
          will be logged, unless you set ``disable_warnings`` explicitly.

        Args:
          array: The :class:`numpy.ndarray` holding the image data.
          copy: If ``True``, the :class:`~viren2d.ImageBuffer` will
            make a deep copy of the given ``array``.
          disable_warnings: If ``True``, warnings about overriding the
            ``copy`` parameter will be silenced.
        )docstr",
        py::arg("array"),
        py::arg("copy") = false,
        py::arg("disable_warnings") = false)
      .def_buffer(&ImageBufferInfo)
      .def(
        "copy",
        &ImageBuffer::DeepCopy, R"docstr(
        Returns a deep copy.

        The returned copy will **always** allocate and copy the memory,
        even if you call this method on a *shared* buffer.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::DeepCopy``.
        )docstr")
      .def(
        "roi",
        &ImageBuffer::ROI, R"docstr(
        Returns an ImageBuffer which points to the given region of interest.

        Allows selecting a rectangular region of interest within this
        :class:`~viren2d.ImageBuffer`. The returned buffer will always
        *share* its memory - be aware of this when performing pixel
        modifications on the ROI afterwards.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::ROI``.

        Args:
           left: Position of the ROI's left edge as :class:`int`.
           top: Position of the ROI's top edge as :class:`int`.
           width: Width of the ROI as :class:`int`.
           height: Height of the ROI as :class:`int`.

        Example:
           >>> roi = painter.canvas.roi(left=10, top=50, width=100, height=200)
        )docstr",
        py::arg("left"), py::arg("top"), py::arg("width"), py::arg("height"))
      .def(
        "is_valid",
        &ImageBuffer::IsValid, R"docstr(
        Returns ``True`` if this buffer points to a valid memory location.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::IsValid``.
        )docstr")
      .def(
        "swap_channels",
        &ImageBuffer::SwapChannels, R"docstr(
        Swaps the specified channels **in-place**.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::SwapChannels``.

        Args:
          ch1: Zero-based index of the first channel as :class:`int`.
          ch2: Zero-based index of the second channel as :class:`int`.
        )docstr", py::arg("ch1"), py::arg("ch2"))
      .def(
        "to_channels",
        &ImageBuffer::ToChannels, R"docstr(
        Returns a copy with duplicated channels or removed alpha channel.

        This method can only **duplicate channels** or **remove the alpha
        channel**. Thus, it only supports the following use cases:

        * From a single channel to 1-, 3-, or 4-channel output.
        * From 3 channels to a 3- or 4-channel output, *i.e.* adding
          an alpha channel.
        * From 4 channels to a 3- or 4-channel output, *i.e.* removing
          the alpha channel.

        It **will not** convert color images to grayscale. For this, use
        :func:`~viren2d.convert_rgb2gray` instead.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::ToChannels``.

        Important:
          This call will always create a deep copy, even if ``self`` already
          has the same number of channels as requested by ``output_channels``.
        )docstr",
        py::arg("output_channels"))
      .def(
        "__repr__",
        [](const ImageBuffer &buf)
        { return "<" + buf.ToString() + ">"; })
      .def("__str__", &ImageBuffer::ToString)
      .def(
        "pixelate",
        &ImageBuffer::Pixelate, R"docstr(
        Pixelates a rectangular region of interest **in-place**.

        Performs **in-place** pixelation of images with **up to 4**
        :attr:`channels`. All pixels within a *block* will be set to
        the value of the block's center pixel.

        If the chosen block size does not align with the region of interest,
        the size of the outer blocks (left, right, top and bottom) will be
        increased to ensure proper pixelation of these areas.

        If ``left``, ``top``, ``width`` **and** ``height`` are all ``-1``,
        the whole image will be pixelated.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::Pixelate``.

        Args:
          block_width: Width of a pixelation block as :class:`int`.
          block_height: Height of a pixelation block as :class:`int`.
          left: Position of the ROI's left edge as :class:`int`.
          top: Position of the ROI's top edge as :class:`int`.
          width: Width of the ROI as :class:`int`.
          height: Height of the ROI as :class:`int`.

        Example:
          >>> img_buf.pixelate(
          >>>     block_width=20, block_height=10,
          >>>     left=100, top=50, width=300, height=150)
        )docstr",
        py::arg("block_width"), py::arg("block_height"),
        py::arg("left") = -1,
        py::arg("top") = -1,
        py::arg("width") = -1,
        py::arg("height") = -1)
      .def(
        "to_uint8",
        &ImageBuffer::ToUInt8, R"docstr(
        Converts this buffer to ``uint8``.

        If the underlying type is :class:`numpy.float32` or :class:`numpy.float64`,
        the values will be **multiplied by 255**. Otherwise, the values will
        be clamped into [0, 255].

        **Corresponding C++ API:** ``viren2d::ImageBuffer::ToUInt8``.

        Args:
          output_channels: Number of output channels as :class:`int`. The following
            configurations are supported:
            * For a single-channel buffer: ``output_channels`` either 1, 3, or 4.
            * For a 3-channel buffer: ``output_channels`` either 3 or 4.
            * For a 4-channel buffer: ``output_channels`` either 3 or 4.
        )docstr", py::arg("output_channels"))
      .def(
        "to_float32",
        &ImageBuffer::ToFloat, R"docstr(
        Converts this buffer to ``float32``.

        If the underlying type is integral (*e.g.* :class:`numpy.uint8`,
        :class:`numpy.int32`), the values will be **divided by 255**.
        The number of channels remains the same.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::ToFloat``.
        )docstr")
      .def(
        "magnitude",
        &ImageBuffer::Magnitude, R"docstr(
        Computes the magnitude along the channels.

        At each spatial location :math:`(r,c)`, this method computes the
        magnitude along the :math:`C` channels, *i.e.*
        :math:`M(r,c) = \sqrt{\sum_{l = [1, \dots, C]}I(r,c,l)^2}`
        Can only be applied to images of type :class:`numpy.float32` or
        :class:`numpy.float64`, *e.g.* optical flow fields or image gradients.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::Magnitude``.
        )docstr")
      .def(
        "orientation",
        &ImageBuffer::Orientation, R"docstr(
        Computes the orientation **in radians** as
        :math:`\operatorname{atan2}\left(I(r, c, 1), I(r, c, 0)\right)`.

        Can only be applied to dual-channel images of type
        :class:`numpy.float32` or :class:`numpy.float64`, *e.g.* optical flow
        fields or image gradients.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::Orientation``.

        Args:
          invalid: If both components of an input pixel are 0, the output value
            will be set to this user-defined `invalid` value.
        )docstr",
        py::arg("invalid") = std::numeric_limits<float>::quiet_NaN())
      .def(
        "channel",
        &ImageBuffer::Channel, R"docstr(
        Extracts a single channel.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::Channel``.

        Args:
          channel: The 0-based channel index as :class:`int`.

        Returns:
          A single-channel :class:`~viren2d.ImageBuffer` holding a deep copy of
          the specified channel.
        )docstr", py::arg("channel"))
      .def(
        "min_max",
        [](const ImageBuffer &buf, int channel) {
          double minval, maxval;
          Vec2i minloc, maxloc;
          buf.MinMaxLocation(&minval, &maxval, &minloc, &maxloc, channel);
          return py::make_tuple(minval, maxval, minloc, maxloc);
        }, R"docstr(
        Computes the min/max values and locations for the given channel.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::MinMaxLocation``.

        Args:
          channel: Zero-based channel index. If called on a single-channel
            buffer, a negative value will automatically changed to 0.
            Otherwise, negative values will raise an :class:`IndexError`.

        Returns:
          A :class:`tuple` which contains ``(min_val, max_val, min_loc, max_loc)``,
          where ``min_val`` & ``max_val`` are the extremal values of the selected
          channel as :class:`float` and ``min_loc`` & ``max_loc`` are the
          :math:`x` and :math:`y` positions as :class:`~viren2d.Vec2i`.
        )docstr",
        py::arg("channel") = -1)
      .def_property_readonly(
        "width",
        &ImageBuffer::Width, R"docstr(
        int: Image width in pixels (read-only).

          **Corresponding C++ API:** ``viren2d::ImageBuffer::Width``.
        )docstr")
      .def_property_readonly(
        "height",
        &ImageBuffer::Height, R"docstr(
        int: Image height in pixels (read-only).

          **Corresponding C++ API:** ``viren2d::ImageBuffer::Height``.
        )docstr")
      .def_property_readonly(
        "channels",
        &ImageBuffer::Channels, R"docstr(
        int: Number of channels (read-only).

          **Corresponding C++ API:** ``viren2d::ImageBuffer::Channels``.
        )docstr")
      .def_property_readonly(
        "row_stride",
        &ImageBuffer::RowStride, R"docstr(
        int: Stride in bytes per row (read-only).

          **Corresponding C++ API:** ``viren2d::ImageBuffer::RowStride``.
        )docstr")
      .def_property_readonly(
        "pixel_stride",
        &ImageBuffer::PixelStride, R"docstr(
        int: Stride in bytes per pixel (read-only).

          **Corresponding C++ API:** ``viren2d::ImageBuffer::PixelStride``.
        )docstr")
      .def_property_readonly(
        "owns_data",
        &ImageBuffer::OwnsData, R"docstr(
        bool: Read-only flag indicating whether this
          :class:`~viren2d.ImageBuffer` owns the image data and is thus
          responsible for cleaning up.

          **Corresponding C++ API:** ``viren2d::ImageBuffer::OwnsData``.
        )docstr")
      .def_property_readonly(
        "shape",
        [](const ImageBuffer &buf) {
          return py::make_tuple(buf.Height(), buf.Width(), buf.Channels());
        }, R"docstr(
        tuple: Shape of the buffer as ``(H, W, C)`` tuple (read-only).

          **No corresponding C++ API**, retrieve height, width and channels
          separately.
        )docstr")
      .def_property_readonly(
        "itemsize",
        &ImageBuffer::ElementSize, R"docstr(
        int: Number of bytes a single buffer element occupies (read-only).

          **Corresponding C++ API:** ``viren2d::ImageBuffer::ElementSize``.
        )docstr")
      .def_property_readonly(
        "dtype",
        [](const ImageBuffer &buf) {
          return py::dtype(FormatDescriptor(buf.BufferType()));
        }, R"docstr(
        numpy.dtype: Underlying data type (read-only).

          **Corresponding C++ API:** ``viren2d::ImageBuffer::BufferType``.
        )docstr");

  imgbuf.def(
        "blend_constant",
        py::overload_cast<const ImageBuffer &, double>(
          &ImageBuffer::Blend, py::const_), R"docstr(
        Returns an alpha-blended image.

        Creates a new image as the result of
        :math:`(1 - \alpha) * \text{self} + \alpha * \text{other}`.
        If the number of channels is not the same, the number of
        output channels will be the maximum of ``self.channels``
        and ``other.channels``. In this case, *non-blendable* channels
        are copied from the input buffer which has more channels.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::Blend``.

        Args:
          other: The other :class:`~viren2d.ImageBuffer` to blend.
          alpha: Blending factor as :class:`float` :math:`\in [0,1]`.
        )docstr",
        py::arg("other"),
        py::arg("alpha"))
      .def(
        "blend_mask",
        py::overload_cast<const ImageBuffer &, const ImageBuffer &>(
          &ImageBuffer::Blend, py::const_), R"docstr(
        Returns an alpha-blended image.

        Creates a new image as the result of
        :math:`(1 - \alpha_{r,c}) * \text{self}_{r,c} + \alpha_{r,c} * \text{other}_{r,c}`,
        where :math:`\alpha` is a weight mask.
        If the mask provides multiple channels, the blending weights will be
        taken from the corresponding channel. Otherwise, the blending weights
        will be taken from the first mask channel.

        If the number of channels of the two images is not the same, the number
        of output channels will be the maximum of ``self.channels``
        and ``other.channels``. In this case, *non-blendable* channels
        are copied from the input buffer which has more channels.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::Blend``.

        Args:
          other: The other :class:`~viren2d.ImageBuffer` to be overlaid.
          alpha: Blending mask/weights as :class:`~viren2d.ImageBuffer`
            of the same width and height. The mask must be of type
            :class:`numpy.float32` or :class:`numpy.float32` and each
            :math:`\alpha_{r,c} \in [0,1]`.

        Example:
          >>> grad = viren2d.LinearColorGradient((0, 0), (img.width, img.height))
          >>> grad.add_intensity_stop(0.1, 1.0)
          >>> grad.add_intensity_stop(0.9, 0.0)
          >>> mask = viren2d.color_gradient_mask(
          >>>     grad, width=img.width, height=img.height)
          >>> blended = img.blend(overlay, mask)
        
        |image-flow-vis-blend|

        Code example to create this blended visualization can be found in the
        :ref:`RTD tutorial section on optical flow colorization<tutorial-optical-flow-blend>`.
        )docstr",
        py::arg("other"),
        py::arg("alpha"));


  imgbuf.def(
        "dim",
        &ImageBuffer::Dim, R"docstr(
        Returns a scaled version of this image as
        :math:`\alpha * \text{self}`.

        This method will not apply any sanity checks to the provided scaling
        factor. Since the output will have the same type, values will be
        implicitly cast to the data type's range. Thus, be aware of potential
        value clipping if :math:`\alpha \notin [0, 1]`.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::Dim``.

        Args:
          alpha: Scaling factor as :class:`float`.

        Example:
          >>> dimmed = img.dim(0.4)
        )docstr",
        py::arg("alpha"));


  // An ImageBuffer can be initialized from a numpy array
  py::implicitly_convertible<py::array, ImageBuffer>();


  m.def("save_image_uint8",
        &SaveImageUInt8Helper, R"docstr(
        Stores an 8-bit image to disk as either JPEG or PNG.

        Important:
          The PNG output will usually result in 20-50% larger files in
          comparison to optimized PNG libraries.
          Thus, this option should only be used if you don't already
          work with a specialized image processing library that offers
          optimized image I/O.

        **Corresponding C++ API:** ``viren2d::SaveImageUInt8``.

        Args:
          filename: The output filename as :class:`str` or
            :class:`pathlib.Path`. The calling code must ensure that the
            directory hierarchy exists.
          image: The :class:`~viren2d.ImageBuffer` which
            should be written to disk.
        )docstr",
        py::arg("filename"), py::arg("image"));


  m.def("load_image_uint8",
        &LoadImageUInt8Helper, R"docstr(
        Reads an 8-bit image from disk.

        This functionality uses the
        `stb library <https://github.com/nothings/stb/blob/master/stb_image.h>`__
        to load the image file. Supported formats are:

           JPEG, PNG, TGA, BMP, PSD, GIF, HDR, PIC, PNM

        **Corresponding C++ API:** ``viren2d::LoadImageUInt8``.

        Args:
          filename: The path to the image file as :class:`str` or
            :class:`pathlib.Path`.
          force_channels: An :class:`int` which is used to
            force the number of loaded channels, *e.g.* to
            load a JPEG into RGBA format with ``force_channels = 4``.

            Valid parameter settings are:

               * ``0``: Load image *as-is*.
               * ``1``: Load image as grayscale.
               * ``2``: Load image as grayscale + alpha channel.
               * ``3``: Load image as RGB.
               * ``4``: Load image as RGBA.
        )docstr",
        py::arg("filename"),
        py::arg("force_channels") = 0);


  m.def("convert_rgb2gray",
        &ConvertRGB2Gray, R"docstr(
        Converts RGB(A)/BGR(A) images to grayscale.

        **Corresponding C++ API:** ``viren2d::ConvertRGB2Gray``.

        Args:
          color: The 3- or 4-channel color :class:`~viren2d.ImageBuffer`
            of type :class:`numpy.uint8`.
          output_channels: Number of output channels as :class:`int`. Must be
            1, 3, or 4. The first three channels will contain the repeated
            luminance, whereas the 4th channel will be the alpha channel and
            either fully opaque or a copy of this image's alpha channel.
          is_bgr: Set to ``True`` if the channels of the color image are in
            BGR format.
        )docstr",
        py::arg("color"),
        py::arg("output_channels") = 1,
        py::arg("is_bgr") = false);


  m.def("convert_gray2rgb",
        [] (const ImageBuffer &g, int output_channels) {
          return g.ToChannels(output_channels);
        }, R"docstr(
        Converts a grayscale image to RGB(A).

        **Corresponding C++ API:** ``viren2d::ImageBuffer::ToChannels``.

        Args:
          grayscale: An :class:`~viren2d.ImageBuffer` which can have 1, 3 or 4
            channels (assuming that the first 3 channels simply repeat the
            luminance and the 4th holds the alpha values).
          output_channels: Number of output channels. Must be 3 (for RGB)
            or 4 (for RGBA).
        )docstr",
        py::arg("grayscale"),
        py::arg("output_channels") = 3);


  m.def("convert_rgb2hsv",
        &ConvertRGB2HSV, R"docstr(
        Converts a RGB(A)/BGR(A) image to HSV.

        **Corresponding C++ API:** ``viren2d::ConvertRGB2HSV``.

        Args:
          image: The 3- or 4-channel color :class:`~viren2d.ImageBuffer`
            of type :class:`numpy.uint8`.
          is_bgr: Set to ``True`` if the channels of the color image are in
            BGR format.

        Returns:
          A 3-channel :class:`~viren2d.ImageBuffer` holding hue, saturation and
          value, with hue :math:`\in [0, 180]`, saturation :math:`\in [0, 255]`
          and value :math:`\in [0, 255]`.
        )docstr",
        py::arg("image"),
        py::arg("is_bgr") = false);


  m.def("convert_hsv2rgb",
        &ConvertHSV2RGB, R"docstr(
        Converts a HSV image to RGB(A)/BGR(A).

        **Corresponding C++ API:** ``viren2d::ConvertHSV2RGB``.

        Args:
          hsv: The 3-channel HSV :class:`~viren2d.ImageBuffer`
            of type :class:`numpy.uint8`, where hue :math:`\in [0, 180]`,
            saturation :math:`\in [0, 255]` and value :math:`\in [0, 255]`.
          output_channels: The number of output channels, which must be 3 or 4.
            The optional fourth channel will be set to 255 (fully-opaque).
          output_bgr: If ``True``, the result will be in BGR(A) format.
        )docstr",
        py::arg("hsv"),
        py::arg("output_channels") = 3,
        py::arg("output_bgr") = false);

//FIXME add python demo + rtd visualization
  m.def("color_pop",
        &ColorPop, R"docstr(
        Returns an image where the specified color range is highlighted.

        Implements a *color pop* effect, *i.e.* colors within the given HSV
        range remain as-is, whereas all other colors are converted to
        grayscale.

        **Corresponding C++ API:** ``viren2d::ColorPop``.

        Args:
          image: Color image in **RGB(A)/BGR(A)** format.
          hue_range: Hue range as :class:`tuple` ``(min_hue, max_hue)``, where
            hue values are of type :class:`float` :math:`\in [0, 360]`.
          saturation_range: Saturation range as :class:`tuple`
            ``(min_saturation, max_saturation)``, where saturation values are
            of type :class:`float` :math:`\in [0, 1]`.
          value_range: Value range as :class:`tuple`
            ``(min_value, max_value)``, where each value is of type
            :class:`float` :math:`\in [0, 1]`.
          is_bgr: Set to ``True`` if the color image is provided in BGR(A)
            format.

        Returns:
          An :class:`~viren2d.ImageBuffer` of type :class:`numpy.uint8`, which
          has the same format and number of channels as the input ``rgb``.

        Example:
          >>> red_pop = viren2d.color_pop(
          >>>     image=img, hue_range=(320, 360), saturation_range=(0.4, 1),
          >>>     value_range=(0.2, 1), is_bgr=False)
        )docstr",
        py::arg("image"),
        py::arg("hue_range"),
        py::arg("saturation_range") = std::make_pair<float, float>(0.0f, 1.0f),
        py::arg("value_range") = std::make_pair<float, float>(0.0f, 1.0f),
        py::arg("is_bgr") = false);
}
} // namespace bindings
} // namespace viren2d

