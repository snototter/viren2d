#include <sstream>
#include <stdexcept>

#include <pybind11/operators.h>
#include <pybind11/numpy.h>

#include <bindings/binding_helpers.h>
#include <viren2d/imagebuffer.h>


namespace py = pybind11;

namespace viren2d {
namespace bindings {
//------------------------------------------------- ImageBuffer from numpy array
inline ImageBufferType ImageBufferTypeFromDType(const pybind11::dtype &dt) {
  if (dt.is(py::dtype::of<uint8_t>())) {
    return ImageBufferType::UInt8;
  } else if (dt.is(py::dtype::of<int16_t>())) {
    return ImageBufferType::Int16;
  } else if (dt.is(py::dtype::of<int32_t>())) {
    return ImageBufferType::Int32;
  } else if (dt.is(py::dtype::of<float>())) {
    return ImageBufferType::Float;
  } else if (dt.is(py::dtype::of<double>())) {
    return ImageBufferType::Double;
  } else {
    std::string s("Buffer `dtype` ");
    s += py::cast<std::string>(dt.attr("name"));
    s += " is not handled in `ImageBufferTypeFromDType`!";
    throw std::logic_error(s);
  }
}


ImageBuffer CreateImageBuffer(py::array buf, bool copy) {
  // Sanity checks
  if (buf.ndim() < 2 || buf.ndim() > 3) {
    std::ostringstream s;
    s << "Incompatible buffer dimensions - "
         "expected `ndim` to be 2 or 3, but got: "
      << buf.ndim() << '!';
    throw std::invalid_argument(s.str());
  }

  // Buffer layout must be row-major (C-style)
  if ((buf.flags() & py::array::c_style) != py::array::c_style) {
    throw std::invalid_argument(
          "An ImageBuffer can only be constructed from C-style buffers! "
          "Check `image_np.flags` and explicitly copy the NumPy array via "
          "`image_np.copy()` before passing it into the ImageBuffer constructor.");
  }

  pybind11::dtype buf_dtype = buf.dtype();
  if (!buf_dtype.is(py::dtype::of<uint8_t>())
      && !buf_dtype.is(py::dtype::of<int16_t>())
      && !buf_dtype.is(py::dtype::of<int32_t>())
      && !buf_dtype.is(py::dtype::of<float>())
      && !buf_dtype.is(py::dtype::of<double>())) {
    std::string s("Incompatible `dtype`: ");
    s += py::cast<std::string>(buf_dtype.attr("name"));
    s += ". ImageBuffer can only be constructed from: "
         "uint8, int16, int32, float32, or float64!";
    // TODO(dev): Update error message with newly supported types, and
    //   extend type handling in `ImageBufferTypeFromDType`!
    //   Also update the docstring of `ImageBuffer`!
    throw std::invalid_argument(s);
  }

  const ImageBufferType buffer_type = ImageBufferTypeFromDType(buf_dtype);
  if (ElementSizeFromImageBufferType(buffer_type) != static_cast<int>(buf_dtype.itemsize())) {
    std::ostringstream s;
    s << "ImageBuffer `" << ImageBufferTypeToString(buffer_type)
      << "` expected item size " << ElementSizeFromImageBufferType(buffer_type)
      << " bytes, but python buffer info states " << buf_dtype.itemsize() << "!";
    throw std::logic_error(s.str());
  }

  ImageBuffer img;
  const int row_stride = static_cast<int>(buf.strides(0));
  const int col_stride = static_cast<int>(buf.strides(1));
  const int height = static_cast<int>(buf.shape(0));
  const int width = static_cast<int>(buf.shape(1));
  const int channels = (buf.ndim() == 2) ? 1 : static_cast<int>(buf.shape(2));

  if (copy) {
    img.CreateCopiedBuffer(
          static_cast<unsigned char const*>(buf.data()),
          height, width, channels, row_stride, col_stride, buffer_type);
  } else {
    img.CreateSharedBuffer(
          static_cast<unsigned char*>(buf.mutable_data()),
          height, width, channels, row_stride, col_stride, buffer_type);
  }

  return img;
}


inline std::string FormatDescriptor(ImageBufferType t) {
  switch (t) {
    case ImageBufferType::UInt8:
      return py::format_descriptor<uint8_t>::format();

    case ImageBufferType::Int16:
      return py::format_descriptor<int16_t>::format();

    case ImageBufferType::Int32:
      return py::format_descriptor<int32_t>::format();

    case ImageBufferType::Float:
      return py::format_descriptor<float>::format();

    case ImageBufferType::Double:
      return py::format_descriptor<double>::format();
  }

  std::string s("ImageBufferType `");
  s += ImageBufferTypeToString(t);
  s += "` not handled in `FormatDescriptor` switch!";
  throw std::logic_error(s);
}


py::buffer_info ImageBufferInfo(ImageBuffer &img) {
  return py::buffer_info(
      img.MutableData(),
      static_cast<std::size_t>(img.ElementSize()), // Size of each element
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


void RegisterImageBuffer(py::module &m) {
  py::class_<ImageBuffer> imgbuf(m, "ImageBuffer", py::buffer_protocol(), R"docstr(
        Encapsulates image data.

        This class is primarily used to pass images between the client
        application and ``viren2d``. Supported data types are:
        :class:`numpy.uint8`, :class:`numpy.int16`, :class:`numpy.int32`,
        :class:`numpy.float32`, and :class:`numpy.float64`.
        Additionally, it provides several basic image manipulation methods
        to adjust an image quickly for visualization.

        The *ImageBuffer* implements the standard Python buffer protocol
        and can thus be swiftly converted to/from other buffer types,
        such as a :class:`numpy.ndarray`, for example:

        >>> # Create an ImageBuffer from a numpy.ndarray
        >>> img_buf = viren2d.ImageBuffer(img_np, copy=False)

        >>> # Create a numpy.ndarray from an ImageBuffer
        >>> img_np = np.array(img_buf, copy=False)
        )docstr");

  imgbuf.def(
        py::init(&CreateImageBuffer), R"docstr(
        Creates an *ImageBuffer* from a :class:`numpy.ndarray`.

        Currently, only conversion from/to NumPy arrays with
        :class:`numpy.dtype` = :class:`numpy.uint8` is supported.
        This will change once I get around to implementing the
        pseudocoloring functionality in ``viren2d``.

        Args:
          array: The :class:`numpy.ndarray` holding the image data.
          copy: If ``True``, the :class:`~viren2d.ImageBuffer` will
            make a copy of the given ``array``. The default (``False``)
            is to share the data instead, which avoids memory allocation.
        )docstr", py::arg("array"), py::arg("copy")=false)
      .def_buffer(&ImageBufferInfo)
      .def(
        "copy",
        &ImageBuffer::DeepCopy, R"docstr(
        Returns a deep copy.

        The returned copy will **always** allocate and copy the memory,
        even if you call this method on a *shared* buffer.
        )docstr")
      .def(
        "roi",
        &ImageBuffer::ROI, R"docstr(
        Returns an ImageBuffer which points to the given region of interest.

        Allows selecting a rectangular region of interest within this
        :class:`~viren2d.ImageBuffer`. The returned buffer will always
        *share* its memory - be aware of this when performing pixel
        modifications on the ROI afterwards.

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
        &ImageBuffer::IsValid,
        "Returns ``True`` if this buffer points to a valid memory location.")
      .def(
        "swap_channels",
        &ImageBuffer::SwapChannels, R"docstr(
        Swaps the specified channels **in-place**.

        Args:
          ch1: Zero-based index of the first channel as :class:`int`.
          ch2: Zero-based index of the second channel as :class:`int`.
        )docstr", py::arg("ch1"), py::arg("ch2"))
      .def(
        "to_rgb",
        [](const ImageBuffer &buf) -> ImageBuffer { return buf.ToChannels(3); }, R"docstr(
        Returns a 3-channel representation.

        This conversion is only supported for :class:`~viren2d.ImageBuffer`
        instances which have 1, 3, or 4 channels. Thus, it will only
        **duplicate**/**copy** channels, or **remove** the alpha channel
        (despite it's name, it is format-agnostic, *i.e.* it doesn't matter
        whether you apply it on a RGB(A) or BGR(A) buffer).

        Note that this call will always allocate and copy memory, even
        if ``self`` is already a 3-channel buffer.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::ToChannels``.
        )docstr")
      .def(
        "to_rgba",
        [](const ImageBuffer &buf) -> ImageBuffer { return buf.ToChannels(4); }, R"docstr(
        Returns a 4-channel representation.

        Refer to :meth:`~viren2d.ImageBuffer.to_rgb` as all comments
        apply analogously.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::ToChannels``.
        )docstr")
      .def(
        "__repr__",
        [](const ImageBuffer &)
        { return "<viren2d.ImageBuffer>"; })
      .def("__str__", &ImageBuffer::ToString)
      .def(
        "pixelate",
        &ImageBuffer::Pixelate, R"docstr(
        Pixelates a rectangular region of interest in-place.

        Performs **in-place** pixelation of images with **up to 4**
        :attr:`channels`. All pixels within a *block* will be set to
        the value of the block's center pixel.

        If the chosen block size does not align with the region of interest,
        the size of the outer blocks (left, right, top and bottom) will be
        increased to ensure proper pixelation of these areas.

        If ``left``, ``top``, ``width`` **and** ``height`` are all ``-1``,
        the whole image will be pixelated.

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

        If the underlying type is `float` or `double`,
        the values will be **multiplied by 255**. Otherwise,
        the values will be clamped into [0, 255].

        Args:
          num_channels: Number of output channels as :class:`int`. The following
            configurations are supported:
            * For a single-channel buffer: ``num_channels`` either 1, 3, or 4.
            * For a 3-channel buffer: ``num_channels`` either 3 or 4.
            * For a 4-channel buffer: ``num_channels`` either 3 or 4.
        )docstr", py::arg("num_channels"))
      .def(
        "to_grayscale",
        &ImageBuffer::ToGrayscale, R"docstr(
        Returns the grayscale image.

        Args:
          num_channels: Number of output channels as :class:`int`, must be `<=4`.
            The first (up to) 3 channels will contain the repeated luminance,
            whereas the 4th channel will always be 255 (*i.e.* alpha, fully opaque).
          is_bgr: Set to ``True`` if the channels of this image are in BGR format.
        )docstr",
        py::arg("num_channels"),
        py::arg("is_bgr") = false)
      .def(
        "channel",
        &ImageBuffer::Channel, R"docstr(
        Extracts a single channel.

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

        Returns:
          A :class:`tuple` which contains ``(min_val, max_val, min_loc, max_loc)``,
          where ``min_val`` & ``max_val`` are the extremal values of the selected
          channel as :class:`float` and ``min_loc`` & ``max_loc`` are the
          *x* & *y* positions as :class:`~viren2d.Vec2i`.
        )docstr")
      .def(
        "blend",
        &ImageBuffer::Blend, R"docstr(
        Returns an alpha-blended image.

        Creates a new image as the result of
        :math:`(1 - \alpha) * self + \alpha * other``.
        If the number of channels is not the same, the number of
        output channels will be the maximum of ``self.channels``
        and ``other.channels``. In this case, *non-blendable* channels
        are copied from the input buffer which has more channels.

        Args:
          other: The other :class:`~viren2d.ImageBuffer` to blend.
          alpha: Blending factor as :class:`float` :math:`\in [0,1]`.
        )docstr", py::arg("other"), py::arg("alpha"))
      .def_property_readonly(
        "width",
        &ImageBuffer::Width,
        "int: Image width in pixels (read-only).")
      .def_property_readonly(
        "height",
        &ImageBuffer::Height,
        "int: Image height in pixels (read-only).")
      .def_property_readonly(
        "channels",
        &ImageBuffer::Channels,
        "int: Number of channels (read-only).")
      .def_property_readonly(
        "row_stride",
        &ImageBuffer::RowStride,
        "int: Stride in bytes per row (read-only).")
      .def_property_readonly(
        "pixel_stride",
        &ImageBuffer::PixelStride,
        "int: Stride in bytes per pixel (read-only).")
      .def_property_readonly(
        "owns_data",
        &ImageBuffer::OwnsData,
        "bool: Read-only flag indicating whether this\n"
        ":class:`~viren2d.ImageBuffer` owns the\n"
        "image data (and is responsible for cleaning up).")
      .def_property_readonly(
        "shape",
        [](const ImageBuffer &buf) {
            return py::make_tuple(
                  buf.Height(), buf.Width(), buf.Channels());
        }, "tuple: Shape of the buffer as ``(H, W, C)`` tuple (read-only).")
      .def_property_readonly(
        "itemsize",
        &ImageBuffer::ElementSize,
        "int: Size (in bytes) of a single buffer element.")
      .def_property_readonly(
        "dtype",
        [](const ImageBuffer &buf) { return py::dtype(FormatDescriptor(buf.BufferType())); },
        "numpy.dtype: Underlying data type (read-only).");


  // An ImageBuffer can be initialized from a numpy array
  py::implicitly_convertible<py::array, ImageBuffer>();


  m.def("save_image",
        &SaveImage, R"docstr(
        Stores an 8-bit image to disk as either JPEG or PNG.

        Note that PNG output will usually result in 20-50% larger files in
        comparison to optimized PNG libraries.
        Thus, this option should only be used if you don't already
        work with a specialized image processing library, which offers
        optimized image I/O.

        Args:
          filename: The output filename as :class:`str`. The
            calling code must ensure that the directory
            hierarchy exists.
          image: The :class:`~viren2d.ImageBuffer` which
            should be written to disk.
        )docstr",
        py::arg("filename"), py::arg("image"));


  m.def("load_image",
        &LoadImage, R"docstr(
        Reads an 8-bit image from disk.

        This functionality uses the
        `stb library <https://github.com/nothings/stb/blob/master/stb_image.h>`__
        to load the image file. Supported formats are:

           JPEG, PNG, TGA, BMP, PSD, GIF, HDR, PIC, PNM

        Args:
          filename: The path to the image file as :class:`str`.
          force_channels: An :class:`int` which is used to
            force the number of loaded channels, *e.g.* to
            load a JPEG as RGBA format with ``force_channels = 4``.

            Valid parameter settings are:

               * ``0``: Load image *as-is*.
               * ``1``: Load image as grayscale.
               * ``2``: Load image as grayscale + alpha channel.
               * ``3``: Load image as RGB.
               * ``4``: Load image as RGBA.
        )docstr",
        py::arg("filename"),
        py::arg("force_channels") = 0);
}
} // namespace bindings
} // namespace viren2d

