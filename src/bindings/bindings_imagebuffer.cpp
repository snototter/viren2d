#include <sstream>
#include <stdexcept>

#include <pybind11/operators.h>
#include <pybind11/numpy.h>

#include <bindings/binding_helpers.h>
#include <viren2d/primitives.h>

namespace py = pybind11;

namespace viren2d {
namespace bindings {
//------------------------------------------------- ImageBuffer from numpy array
// We need a uint8, row-major (C-style) numpy array:
ImageBuffer CreateImageBuffer(
    py::array_t<unsigned char, py::array::c_style | py::array::forcecast> buf,
    bool copy) {
  // Sanity checks
  if (buf.ndim() < 2 || buf.ndim() > 3)
    throw std::runtime_error("Incompatible image dimension!");

  if (!buf.dtype().is(py::dtype::of<uint8_t>()))
    throw std::runtime_error("Incompatible format: expected a uint8 array!");

  ImageBuffer img;
  const int row_stride = static_cast<int>(buf.strides(0));
  const int height = static_cast<int>(buf.shape(0));
  const int width = static_cast<int>(buf.shape(1));
  const int channels = (buf.ndim() == 2) ? 1 : static_cast<int>(buf.shape(2));
  if (copy) {
    img.CreateCopy(buf.data(), width, height, channels, row_stride);
  } else {
    img.CreateSharedBuffer(buf.mutable_data(), width, height, channels, row_stride);
  }
  return img;
}

void RegisterImageBuffer(py::module &m) {
  //FIXME update docstring once ImageBuffer supports other data types

  py::class_<ImageBuffer>(m, "ImageBuffer", py::buffer_protocol(), R"docstr(
          An :class:`~viren2d.ImageBuffer` holds 8-bit image data.

          This class is used to pass image data between your
          application and ``viren2d``. Currently, only grayscale,
          RGB, and RGBA images of type uint8 are supported.

          Note that :class:`~viren2d.ImageBuffer` implements the
          Python buffer protocol and thus, can be swiftly
          converted to/from other buffer types, such as NumPy
          arrays, for example:

          >>> # Create an ImageBuffer from a numpy.ndarray
          >>> img_buf = viren2d.ImageBuffer(img_np, copy=False)
          >>> # Create a numpy.ndarray from an ImageBuffer
          >>> img_np = np.array(img_buf, copy=False)
          )docstr")
      .def(py::init(&CreateImageBuffer), R"docstr(
          Creates an :class:`~viren2d.ImageBuffer` from a :class:`numpy.ndarray`.

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
     .def_buffer([](ImageBuffer &img) -> py::buffer_info {
          return py::buffer_info(
              img.data, sizeof(unsigned char), // Pointer to data & size of each element
              py::format_descriptor<unsigned char>::format(), // Python struct-style format descriptor
              3, // We'll always return ndim=3 arrays
              { static_cast<size_t>(img.height),
                static_cast<size_t>(img.width),
                static_cast<size_t>(img.channels) }, // Buffer dimensions
              { static_cast<size_t>(img.stride),
                static_cast<size_t>(img.channels),
                sizeof(unsigned char) } // Strides (in bytes) per dimension
          );
      })
      .def("copy", [](const ImageBuffer &buf) {
             ImageBuffer cp;
             cp.CreateCopy(buf.data, buf.width, buf.height, buf.channels, buf.stride);
             return cp;
           }, R"docstr(
        Returns a deep copy.

        The returned copy will **always** allocate and copy the memory,
        even if you call this method on a *shared* buffer.
        )docstr")
      .def("is_valid", &ImageBuffer::IsValid,
           "Returns ``True`` if this buffer points to a valid memory location.")
      .def("flip_channels", &ImageBuffer::SwapChannels, R"docstr(
           Swaps the specified channels **in-place**.

           Args:
             ch1: Zero-based index of the first channel as :class:`int`.
             ch2: Zero-based index of the second channel as :class:`int`.
           )docstr", py::arg("ch1"), py::arg("ch2"))
      .def("to_rgb", [](const ImageBuffer &buf) -> ImageBuffer { return buf.ToChannels(3); }, R"docstr(
           Returns a 3-channel representation.

           This conversion is only supported for :class:`~viren2d.ImageBuffer`
           instances which have 1, 3, or 4 channels.
           Note that this call will always allocate and copy memory, even
           if ``self`` is already a 3-channel buffer.
           )docstr")
      .def("to_rgba", [](const ImageBuffer &buf) -> ImageBuffer { return buf.ToChannels(4); }, R"docstr(
           Returns a 4-channel representation.

           This conversion is only supported for :class:`~viren2d.ImageBuffer`
           instances which have 1, 3, or 4 channels.
           Note that this call will always allocate and copy memory, even
           if ``self`` is already a 4-channel buffer.
           )docstr")
      .def("__repr__",
           [](const ImageBuffer &)
           { return FullyQualifiedType("ImageBuffer", true); })
      .def("__str__", &ImageBuffer::ToString)
      .def_readonly("width", &ImageBuffer::width,
           "int: Image width in pixels (read-only).")
      .def_readonly("height", &ImageBuffer::height,
           "int: Image height in pixels (read-only).")
      .def_readonly("channels", &ImageBuffer::channels,
           "int: Number of channels (read-only).")
      .def_readonly("stride", &ImageBuffer::stride,
           "int: Stride in bytes per row  (read-only).")
      .def_property_readonly("owns_data", &ImageBuffer::OwnsData,
           "bool: Read-only flag indicating whether this\n"
           ":class:`~viren2d.ImageBuffer` owns the\n"
           "image data (and is responsible for cleaning up).");

  // An ImageBuffer can be initialized from a numpy array
  py::implicitly_convertible<py::array, ImageBuffer>();
}
} // namespace bindings
} // namespace viren2d

