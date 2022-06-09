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
    py::array_t<unsigned char, py::array::c_style | py::array::forcecast> buf) {
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
  img.CreateSharedBuffer(buf.mutable_data(), width, height, channels, row_stride);
  return img;
}

void RegisterImageBuffer(py::module &m) {
  // Info on numpy memory layout: https://stackoverflow.com/a/53099870/400948
  py::class_<ImageBuffer>(m, "ImageBuffer", py::buffer_protocol(),
           "An ImageBuffer holds 8-bit images (Grayscale,\n"
           "RGB or RGBA).")
      .def(py::init(&CreateImageBuffer),
           "Initialize from numpy array with dtype uint8.")
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
           }, "Returns a deep copy. This new ImageBuffer will always allocate\n"
           "and copy the memory, even if you call copy() on a \"shared\" buffer.")
      .def("is_valid", &ImageBuffer::IsValid,
           "Returns True if this buffer points to a valid memory location.")
      .def("flip_channels", &ImageBuffer::RGB2BGR,
           "Swap red and blue color channels in-place!")
      .def("to_rgb", &ImageBuffer::ToRGB,
           "Convert to RGB. Will always return a copy, even if this buffer\n"
           "is already RGB.")
      .def("to_rgba", &ImageBuffer::ToRGBA,
           "Convert to RGBA. Will always return a copy, even if this buffer\n"
           "is already RGBA.")
      .def("__repr__",
           [](const ImageBuffer &b)
           { return FullyQualifiedType(b.ToString(), true); })
      .def("__str__", &ImageBuffer::ToString)
      .def_readonly("width", &ImageBuffer::width,
           "Image width in pixels.")
      .def_readonly("height", &ImageBuffer::height,
           "Image height in pixels.")
      .def_readonly("channels", &ImageBuffer::channels,
           "Number of channels.")
      .def_readonly("stride", &ImageBuffer::stride,
           "Stride per row (in bytes).")
      .def_readonly("owns_data", &ImageBuffer::owns_data_,
           "Boolean flag indicating if this ImageBuffer is\n"
           "the owner of the image data (and is thus responsible\n"
           "for memory cleanup).");

  // An ImageBuffer can be initialized from a numpy array
  py::implicitly_convertible<py::array, ImageBuffer>();
}
} // namespace bindings
} // namespace viren2d

