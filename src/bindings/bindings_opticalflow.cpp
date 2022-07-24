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


ImageBuffer LoadOpticalFlowHelper(const py::object &path) {
  return LoadOpticalFlow(PathStringFromPyObject(path));
}


void SaveOpticalFlowHelper(const py::object &path, const ImageBuffer &flow) {
  SaveOpticalFlow(PathStringFromPyObject(path), flow);
}


void RegisterOpticalFlowUtils(py::module &m) {
  m.def("save_optical_flow",
        &SaveOpticalFlowHelper, R"docstr(
        Saves a two-band optical flow field as ``.flo`` file.

        **Corresponding C++ API:** ``viren2d::SaveOpticalFlow``.

        Args:
          filename: The output filename as :class:`str` or
            :class:`pathlib.Path`. The calling code must ensure that the
            directory hierarchy exists.
          flow: The optical flow data as 2-channel
            :class:`~viren2d.ImageBuffer` which should be written to disk.
            Must be of type :class:`numpy.float32` or :class:`numpy.float64`.
        )docstr",
        py::arg("filename"), py::arg("flow"));


  m.def("load_optical_flow",
        &LoadOpticalFlowHelper, R"docstr(
        Loads an optical flow field from a ``.flo`` file.

        **Corresponding C++ API:** ``viren2d::LoadOpticalFlow``.

        Args:
          filename: The path to the ``.flo`` file as :class:`str` or
            :class:`pathlib.Path`.

        Returns:
          A 2-channel :class:`~viren2d.ImageBuffer` of
          type :class:`numpy.float32`.
        )docstr",
        py::arg("filename"));


  m.def("colorize_optical_flow",
        &OpticalFlowColorizationHelper, R"docstr(
        Colorizes a two-band optical flow field.

        Given a cyclic color map, this returns the false color representation,
        where the flow orientation defines the color map bin and the magnitude
        defines the corresponding color's saturation.
        The default color map is the cyclic six-color map CET-C6 proposed by
        `Peter Kovesi <https://arxiv.org/abs/1509.03700>`__.

        This assumes that flow is normalized such that the maximum magnitude
        is 1. Larger motion will be indicated by a desaturated color. To avoid
        this, you can adjust the ``motion_normalizer`` parameter.

        **Corresponding C++ API:** ``viren2d::ColorizeOpticalFlow``.

        Args:
          flow: The optical flow field as 2-channel :class:`~viren2d.ImageBuffer`,
            where the first and second channels hold the motion in :math:`x`
            and :math:`y` direction, respectively. Must be of type
            :class:`numpy.float32` or :class:`numpy.float64`.
          colormap: The :class:`~viren2d.ColorMap` to be used for
            colorization. In addition to the enumeration value, the corresponding
            string representation can be used for convenience.
          motion_normalizer: A :class:`float` parameter used to divide the flow
            magnitude. Set to the maximum motion magnitude to avoid
            desaturation in regions where the flow magnitude would be :math:`> 1`.
          output_channels: Number of output channels. Must be either 3 or 4.
            If a fourth channel is requested, it is considered an alpha
            channel and set to 255.

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
        Returns the color wheel visualization.

        **Corresponding C++ API:** ``viren2d::OpticalFlowLegend``.

        Args:
          size: The output image will be ``size`` by ``size`` pixels.
          colormap: The :class:`~viren2d.ColorMap` to be used for
            colorization. In addition to the enumeration value, the corresponding
            string representation can be used for convenience.
          line_style: A :class:`~viren2d.LineStyle` specifying how to draw the
            grid overlay on the legend. Disable grid overlay by passing
            :attr:`LineStyle.Invalid`.
          draw_circle: If the grid is overlaid (see ``line_style``), you can
            overlay a circle by setting this flag to ``True``).
          clip_circle: If ``output_channels`` is 4 and this flag is ``True``,
            the color wheel legend will be clipped by a circle within the alpha
            channel.
          output_channels: Number of output channels, must be either 3 or 4.
            If a fourth channel is requested, it is considered an alpha
            channel. Alpha values depend on the value of ``clip_circle``.

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

