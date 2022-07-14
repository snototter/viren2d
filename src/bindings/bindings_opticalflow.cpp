#include <sstream>
#include <stdexcept>
#include <limits>

#include <bindings/binding_helpers.h>

#include <viren2d/opticalflow.h>

namespace py = pybind11;

namespace viren2d {
namespace bindings {

ImageBuffer OpticalFlowColorizationHelper(
    const ImageBuffer &flow, const py::object &colormap,
    double motion_normalizer, int output_channels) {
  ColorMap cmap = ColorMapFromPyObject(colormap);
  return ColorizeOpticalFlow(
        flow, cmap, motion_normalizer, output_channels);
}


ImageBuffer OpticalFlowLegendHelper(
    int size, const py::object &colormap, LineStyle line_style,
    bool draw_circle, bool clip_circle, int output_channels) {
  ColorMap cmap = ColorMapFromPyObject(colormap);
  return OpticalFlowLegend(
        size, cmap, line_style, draw_circle, clip_circle, output_channels);
}


void RegisterOpticalFlowUtils(pybind11::module &m) {
  m.def("save_optical_flow",
        &SaveOpticalFlow, R"docstr(
        Saves a two-band optical flow field as `.flo` format.

        Args:
          filename: The output filename as :class:`str`. The
            calling code must ensure that the directory
            hierarchy exists.
          flow: The :class:`~viren2d.ImageBuffer` which
            should be written to disk.
        )docstr",
        py::arg("filename"), py::arg("flow"));


  m.def("load_optical_flow",
        &LoadOpticalFlow, R"docstr(
        Loads an optical flow field from a ``.flo`` file.

        Args:
          filename: The path to the ``.flo`` file as :class:`str`.
        )docstr",
        py::arg("filename"));


  m.def("colorize_optical_flow",
        &OpticalFlowColorizationHelper, R"docstr(
        TODO doc

        The default color map is the cyclic six-color map CET-C6 proposed by
        `Peter Kovesi <https://arxiv.org/abs/1509.03700>`__.

        Args:
          flow: TODO
          colormap: The :class:`~viren2d.ColorMap` to be used for
            colorization. In addition to the enum value, the corresponding
            string representation can be used for convenience.
          motion_normalizer: TODO :class:`float`
          output_channels: Number of output channels, must be either 3 or 4.

        Returns:
          A 3- or 4-channel :class:`~viren2d.ImageBuffer` of
          type :class:`numpy.uint8`.
        )docstr",
        py::arg("flow"),
        py::arg("colormap") = ColorMap::Orientation6,
        py::arg("motion_normalizer") = 1.0,
        py::arg("output_channels") = 3);


  m.def("optical_flow_legend",
        &OpticalFlowLegendHelper, R"docstr(
        TODO

        Args:
          size: :class:`int`
          colormap: The :class:`~viren2d.ColorMap` to be used for
            colorization. In addition to the enum value, the corresponding
            string representation can be used for convenience.
          line_style: A :class:`~viren2d.LineStyle` specifying how to draw the
            grid overlay on the legend.
          draw_circle: :class:`bool` TODO
          clip_circle: todo - if channels == 4 --> clip, otherwise no effect
          output_channels: Number of output channels, must be either 3 or 4.

        Returns:
          A 3- or 4-channel :class:`~viren2d.ImageBuffer` of
          type :class:`numpy.uint8`.
        )docstr",
        py::arg("size"),
        py::arg("colormap") = ColorMap::Orientation6,
        py::arg("line_style") = LineStyle::Invalid,
        py::arg("draw_circle") = false,
        py::arg("clip_circle") = false,
        py::arg("output_channels") = 3);
}
} // namespace bindings
} // namespace viren2d

