#include <sstream>
#include <stdexcept>

#include <pybind11/stl.h>

#include <bindings/binding_helpers.h>
#include <viren2d/collage.h>

#include <helpers/logging.h>

namespace py = pybind11;

namespace viren2d {
namespace bindings {

template <typename Container>
std::vector<ImageBuffer> ExtractImageBuffers(
    const Container &img_container,
    std::size_t row_idx) {
  std::vector<ImageBuffer> images;
  for (py::handle img : img_container) {
    if (py::isinstance<ImageBuffer>(img)) {
      images.push_back(img.cast<ImageBuffer>());
    } else if (py::isinstance<py::array>(img)) {
      images.push_back(CreateImageBufferUint8C4(img.cast<py::array>()));
    } else {
      const std::string tp = py::cast<std::string>(
        img.attr("__class__").attr("__name__"));
      std::ostringstream msg;
      msg << "Image input to `collage` at row "
          << row_idx << ", column " << images.size()
          << " must be a `numpy.ndarray` or `viren2d.ImageBuffer`, but got `"
          << tp << "`!";
      SPDLOG_ERROR(msg.str());
      throw std::invalid_argument(msg.str());
    }
  }
  return images;
}


template <typename Container>
std::vector<std::vector<ImageBuffer>> ExtractImageBufferRows(
    const Container rows) {
  std::vector<std::vector<ImageBuffer>> images;
  for (py::handle row : rows) {
    if (py::isinstance<py::list>(row)) {
      images.push_back(
          ExtractImageBuffers<py::list>(
              row.cast<py::list>(),
              images.size()));
    } else if (py::isinstance<py::tuple>(row)) {
      images.push_back(
          ExtractImageBuffers<py::tuple>(
              row.cast<py::tuple>(), images.size()));
    } else {
      const std::string tp = py::cast<std::string>(
        row.attr("__class__").attr("__name__"));
      std::ostringstream msg;
      msg << "Input to `collage` at row " << images.size()
          << " must be a `list` or `tuple`, but got `"
          << tp << "`!";
      SPDLOG_ERROR(msg.str());
      throw std::invalid_argument(msg.str());
    }
  }
  return images;
}

//TODO change py::object& to py::object (they're py::handles and thus refs anyhow)
/*
import numpy as np
import viren2d
viren2d.collage([[np.ones((3, 3)), np.zeros((5, 7))]])
viren2d.collage(['a'])
viren2d.collage([('a',), [1, 2]])
*/
ImageBuffer CollageWrapper(
    const py::object py_images,
    const Vec2i &image_size,
    const py::object py_halign,
    const py::object py_valign,
    const Color &bg_color,
    const Vec2i &spacing,
    const Vec2i &margin) {
  HorizontalAlignment halign = HorizontalAlignmentFromPyObject(py_halign);
  VerticalAlignment valign = VerticalAlignmentFromPyObject(py_valign);

  std::vector<std::vector<ImageBuffer>> images;
  if (py::isinstance<py::list>(py_images)) {
    images = ExtractImageBufferRows<py::list>(
          py_images.cast<py::list>());
  } else if (py::isinstance<py::tuple>(py_images)) {
    images = ExtractImageBufferRows<py::tuple>(
          py_images.cast<py::tuple>());
  } else {
    const std::string tp = py::cast<std::string>(
        py_images.attr("__class__").attr("__name__"));
    std::string msg("Input to `collage` must be a container (list/tuple), but got ");
    msg += tp;
    msg += '!';
    SPDLOG_ERROR(msg);
    throw std::invalid_argument(msg);
  }
  return Collage(
      images, image_size, halign, valign, bg_color, spacing, margin);
}


void RegisterCollage(py::module &m) {
  m.def("collage",
        &CollageWrapper, R"docstr(
        TODO
        )docstr",
        py::arg("images"),
        py::arg("img_size") = Vec2i{-1, -1},
        py::arg("halign") = HorizontalAlignment::Left,
        py::arg("valign") = VerticalAlignment::Top,
        py::arg("fill_color") = Color::White,
        py::arg("spacing") = Vec2i{0, 0},
        py::arg("margin") = Vec2i{0, 0});
}
} // namespace bindings
} // namespace viren2d

