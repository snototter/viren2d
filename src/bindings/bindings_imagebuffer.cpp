#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <limits>

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
  } else if (dt.is(py::dtype::of<uint16_t>())) {
    return ImageBufferType::UInt16;
  } else if (dt.is(py::dtype::of<int32_t>())) {
    return ImageBufferType::Int32;
  } else if (dt.is(py::dtype::of<uint32_t>())) {
    return ImageBufferType::UInt32;
  } else if (dt.is(py::dtype::of<int64_t>())) {
    return ImageBufferType::Int64;
  } else if (dt.is(py::dtype::of<uint64_t>())) {
    return ImageBufferType::UInt64;
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

  const pybind11::dtype buf_dtype = buf.dtype();
  if (!buf_dtype.is(py::dtype::of<uint8_t>())
      && !buf_dtype.is(py::dtype::of<int16_t>())
      && !buf_dtype.is(py::dtype::of<uint16_t>())
      && !buf_dtype.is(py::dtype::of<int32_t>())
      && !buf_dtype.is(py::dtype::of<uint32_t>())
      && !buf_dtype.is(py::dtype::of<int64_t>())
      && !buf_dtype.is(py::dtype::of<uint64_t>())
      && !buf_dtype.is(py::dtype::of<float>())
      && !buf_dtype.is(py::dtype::of<double>())) {
    std::string s("Incompatible `dtype`: ");
    s += py::cast<std::string>(buf_dtype.attr("name"));
    s += ". ImageBuffer can only be constructed from: "
         "uint8, (u)int16, (u)int32, (u)int64, float32, or float64!";
    // TODO(dev): Update error message with newly supported types, and
    //   extend type handling in `ImageBufferTypeFromDType`.
    //   Also update the docstring of the `ImageBuffer` class.
    throw std::invalid_argument(s);
  }

  // Buffer layout must be row-major (C-style)
  if ((buf.flags() & py::array::c_style) != py::array::c_style) {
    throw std::invalid_argument(
          "An ImageBuffer can only be constructed from C-style buffers! "
          "Check `image_np.flags` and explicitly copy the NumPy array via "
          "`image_np.copy()` before passing it into the ImageBuffer constructor.");
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


std::string PathFromPyObject(const py::object &path) {
  if (py::isinstance<py::str>(path)) {
    return path.cast<std::string>();
  } else {
    return py::str(path).cast<std::string>();
  }
}


ImageBuffer LoadImageUInt8Helper(
    const py::object &path, int force_num_channels) {
  return LoadImageUInt8(PathFromPyObject(path), force_num_channels);
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

        >>> # Create an ImageBuffer from a numpy.ndarray
        >>> img_buf = viren2d.ImageBuffer(img_np, copy=False)

        >>> # Create a numpy.ndarray from an ImageBuffer
        >>> img_np = np.array(img_buf, copy=False)

        Important:
           A :class:`numpy.ndarray` can be implicitly converted to an
           :class:`~viren2d.ImageBuffer`. Thus, there is no need for explicit
           conversion when calling a ``viren2d`` function which expects an
           :class:`~viren2d.ImageBuffer`.
           The only caveat is that the :class:`numpy.ndarray` must be
           **row-major** (*i.e.* C-style) and **contiguous**. If you need to
           pass the result of a slicing operation, you'll need to explicitly
           create a copy via :meth:`numpy.ndarray.copy` first.
        )docstr");

  imgbuf.def(
        py::init(&CreateImageBuffer), R"docstr(
        Creates an *ImageBuffer* from a :class:`numpy.ndarray`.

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

        **Corresponding C++ API:** ``viren2d::ImageBuffer::ToChannels(3)``.
        )docstr")
      .def(
        "to_rgba",
        [](const ImageBuffer &buf) -> ImageBuffer { return buf.ToChannels(4); }, R"docstr(
        Returns a 4-channel representation.

        Refer to :meth:`~viren2d.ImageBuffer.to_rgb` as all comments
        apply analogously.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::ToChannels(4)``.
        )docstr")
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
        Computes the magnitude for dual-channel floating point images.

        Can only be applied to dual-channel images of type
        :class:`numpy.float32` or :class:`numpy.float64`, *e.g.* optical flow
        fields or image gradients.

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

        Returns:
          A :class:`tuple` which contains ``(min_val, max_val, min_loc, max_loc)``,
          where ``min_val`` & ``max_val`` are the extremal values of the selected
          channel as :class:`float` and ``min_loc`` & ``max_loc`` are the
          *x* & *y* positions as :class:`~viren2d.Vec2i`.
        )docstr",
        py::arg("channel"))
      .def(
        "blend",
        &ImageBuffer::Blend, R"docstr(
        Returns an alpha-blended image.

        Creates a new image as the result of
        :math:`(1 - \alpha) * \text{self} + \alpha * \text{other}``.
        If the number of channels is not the same, the number of
        output channels will be the maximum of ``self.channels``
        and ``other.channels``. In this case, *non-blendable* channels
        are copied from the input buffer which has more channels.

        **Corresponding C++ API:** ``viren2d::ImageBuffer::Blend``.

        Args:
          other: The other :class:`~viren2d.ImageBuffer` to blend.
          alpha: Blending factor as :class:`float` :math:`\in [0,1]`.
        )docstr", py::arg("other"), py::arg("alpha"))
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


  // An ImageBuffer can be initialized from a numpy array
  py::implicitly_convertible<py::array, ImageBuffer>();


  m.def("save_image_uint8",
        &SaveImageUInt8, R"docstr(
        Stores an 8-bit image to disk as either JPEG or PNG.

        Note that PNG output will usually result in 20-50% larger files in
        comparison to optimized PNG libraries.
        Thus, this option should only be used if you don't already
        work with a specialized image processing library, which offers
        optimized image I/O.

        **Corresponding C++ API:** ``viren2d::SaveImageUInt8``.

        Args:
          filename: The output filename as :class:`str`. The
            calling code must ensure that the directory
            hierarchy exists.
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
          filename: The path to the image file as :class:`str`.
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


  m.def("color_pop",
        &ColorPop, R"docstr(
        Returns an image where the specified color range is highlighted.

        Implements a *color pop* effect, *i.e.* colors within the given HSV
        range remain as-is, whereas all other colors are converted to
        grayscale.

        **Corresponding C++ API:** ``viren2d::ColorPop``.

        Args:
          rgb: Color image in RGB(A)/BGR(A) format as
            :class:`~viren2d.ImageBuffer`.
          hue_range: Hue range as :class:`tuple` ``(min_hue, max_hue)``, where
            hue values are of type :class:`float` and :math:`\in [0, 360]`.
          saturation_range: Saturation range as :class:`tuple`
            ``(min_saturation, max_saturation)``, where saturation values are
            of type :class:`float` and :math:`\in [0, 1]`.
          value_range: Value range as :class:`tuple`
            ``(min_value, max_value)``, where each value is of type
            :class:`float` and :math:`\in [0, 1]`.
          is_bgr: Set to ``True`` if the color image is provided in BGR(A)
            format.

        Returns:
          An :class:`~viren2d.ImageBuffer` of type :class:`numpy.uint8`, which
          has the same format and number of channels as the input ``rgb``.

        Example:
          >>> red_pop = viren2d.color_pop(
          >>>     rgb=img, hue_range=(320, 360), saturation_range=(0.4, 1),
          >>>     value_range=(0.2, 1), is_bgr=False)
        )docstr",
        py::arg("rgb"),
        py::arg("hue_range"),
        py::arg("saturation_range") = std::make_pair<float, float>(0.0f, 1.0f),
        py::arg("value_range") = std::make_pair<float, float>(0.0f, 1.0f),
        py::arg("is_bgr") = false);

}
} // namespace bindings
} // namespace viren2d

