#include <sstream>
#include <stdexcept>
#include <limits>

#include <pybind11/stl.h>

#include <bindings/binding_helpers.h>

#include <viren2d/colorgradients.h>

namespace py = pybind11;

namespace viren2d {
namespace bindings {
namespace gradients {
void RegisterColorGradientBase(pybind11::module &m) {
  py::class_<ColorGradient> grad(m, "ColorGradient", R"docstr(
      Base class for :class:`~viren2d.LinearColorGradient`
      and :class:`~viren2d.RadialColorGradient`.

      This should not be instantiated directly, refer to the
      derived classes instead.

      **Corresponding C++ API:** ``viren2d::ColorGradient``.
      )docstr");


  grad.def(
      "__repr__",
      [](const ColorGradient &g)
      { return "<" + g.ToString() + ">"; })
    .def("__str__", &ColorGradient::ToString);


  grad.def(
      "is_valid",
      &ColorGradient::IsValid, R"docstr(
      Returns true if this color gradient can be rendered, *i.e.* if it has
      at least 2 color stops.
      )docstr");


  grad.def(
        "add_color_stop",
        &ColorGradient::AddColorStop, R"docstr(
        Adds a color stop to the gradient.

        Args:
          offset: The offset :math:`\in [0,1]` is the location along the
            gradient's control vector.
          color: The :class:`~viren2d.Color` at the specified offset.

        Returns:
          True if the color stop was added, false if the inputs were invalid
          (for example, out-of-range) which will be detailed in a logged
          warning message.

        Example:
          >>> grad.add_color_stop(0.1, 'crimson')
          >>> grad.add_color_stop(0.9, 'navy-blue')
        )docstr",
        py::arg("offset"),
        py::arg("color"));


  grad.def(
        "add_intensity_stop",
        &ColorGradient::AddIntensityStop, R"docstr(
        Adds an intensity/grayscale stop to the gradient.

        Args:
          offset: The offset :math:`\in [0,1]` is the location along the
            gradient's control vector.
          color: The intensity/grayscale value :math:`\in [0,1]` at the
            specified offset.
          alpha: The opacity :math:`\in [0,1]` at this stop.

        Returns:
          True if the intensity stop was added, false if the inputs were
          invalid (for example, out-of-range) which will be detailed in a
          logged warning message.

        Example:
          >>> grad.add_intensity_stop(0.1, 0.0)
          >>> grad.add_intensity_stop(0.9, 1.0)
        )docstr",
        py::arg("offset"),
        py::arg("intensity"),
        py::arg("alpha") = 1.0);
}


void RegisterLinearColorGradient(pybind11::module &m) {
  py::class_<LinearColorGradient, ColorGradient> grad(
      m, "LinearColorGradient", R"docstr(
      Gradient definition for linear color blending along a line.

      Represents a linear gradient along the control vector (used to
      add color stops) from a line's start point to its end point.
      After initialization, the color stops have to
      be added via :meth:`~viren2d.LinearColorGradient.add_color_stop` or
      :meth:`~viren2d.LinearColorGradient.add_intensity_stop`.

      **Corresponding C++ API:** ``viren2d::LinearColorGradient``.

      Example:
        >>> grad = viren2d.LinearColorGradient((0, 0), (200, 200))
        >>> grad.add_color_stop(0.1, 'crimson')
        >>> grad.add_color_stop(0.5, 'teal-green')
        >>> grad.add_color_stop(0.9, 'navy-blue')
      )docstr");

  grad.def(
      py::init<const Vec2d&, const Vec2d>(), R"docstr(
      Initializes the gradient for linear blending along the line.

      Args:
        pt1: Start position, *i.e.* at :math:`\text{offset} = 0`,
          as :class:`~viren2d.Vec2d`.
        pt2: End position, *i.e.* at :math:`\text{offset} = 1`,
          as :class:`~viren2d.Vec2d`.
      )docstr",
      py::arg("pt1"),
      py::arg("pt2"));
}


void RegisterRadialColorGradient(pybind11::module &m) {
  py::class_<RadialColorGradient, ColorGradient> grad(
      m, "RadialColorGradient", R"docstr(
      Radial gradient between two circles.

      Represents a radial gradient between two circles.
      The control vector (for adding color stops) is from any point on
      :math:`\text{circle}_1` to the corresponding point on
      :math:`\text{circle}_2`.
      After initialization, the color stops have to
      be added via :meth:`~viren2d.RadialColorGradient.add_color_stop` or
      :meth:`~viren2d.RadialColorGradient.add_intensity_stop`.

      **Corresponding C++ API:** ``viren2d::RadialColorGradient``.

      Example:
        >>> grad = viren2d.RadialColorGradient(
        >>>   (50, 50), 10, (50, 50), 40)
        >>> grad.add_color_stop(0.0, 'freesia')
        >>> grad.add_color_stop(0.9, 'navy-blue')
      )docstr");

  grad.def(
      py::init<const Vec2d&, double, const Vec2d, double>(), R"docstr(
      Initializes the radial gradient.

      Args:
        center1: Center of :math:`\text{circle}_1` as :class:`~viren2d.Vec2d`.
        radius1: Radius of :math:`\text{circle}_1`.
        center2: Center of :math:`\text{circle}_2` as :class:`~viren2d.Vec2d`.
        radius2: Radius of :math:`\text{circle}_2`.
      )docstr",
      py::arg("center1"),
      py::arg("radius1"),
      py::arg("center2"),
      py::arg("radius2"));
}

} // namespace gradients

void RegisterColorGradients(pybind11::module &m) {
  gradients::RegisterColorGradientBase(m);
  gradients::RegisterLinearColorGradient(m);
  gradients::RegisterRadialColorGradient(m);
  //TODO bind Painter::DrawGradient
  //TODO bind Painter::SetClipRegion

  m.def("color_gradient_visualization",
        &CreateColorGradientVisualization, R"docstr(
        Renders a color gradient onto an image of the specified size.

        **Corresponding C++ API:** ``viren2d::CreateColorGradientVisualization``.

        Args:
          gradient: The :class:`~viren2d.ColorGradient` to be rendered.
          width: Width of the output image in pixels.
          height: Height of the output image in pixels.
          channels: Number of output channels, must be either 3 or 4.
          fill_color: The :class:`~viren2d.Color` used to initialize the
            output image before rendering the gradient.

        Returns:
          A 3- or 4-channel :class:`~viren2d.ImageBuffer` of type
          :class:`numpy.uint8` as the result of rendering the given gradient.

        Example:
          >>> grad = viren2d.LinearColorGradient((0, 0), (200, 200))
          >>> grad.add_color_stop(0.1, 'crimson!80')
          >>> grad.add_color_stop(0.5, 'teal-green!60')
          >>> grad.add_color_stop(0.9, 'navy-blue!80')
          >>> vis = viren2d.color_gradient_visualization(
          >>>   grad, width=200, height=200, channels=3, fill_color="white")
        )docstr",
        py::arg("gradient"),
        py::arg("width"),
        py::arg("height"),
        py::arg("channels") = 3,
        py::arg("fill_color") = Color::White);


  m.def("color_gradient_mask",
        &CreateColorGradientMask, R"docstr(
        Returns a single-channel double-precision mask for the given gradient.

        **Corresponding C++ API:** ``viren2d::CreateColorGradientMask``.

        Args:
          gradient: The :class:`~viren2d.ColorGradient` to be rendered. Only
            the red and alpha components of its color stops will be contribute
            to the output mask.
          width: Width of the output mask in pixels.
          height: Height of the output mask in pixels.
          fill_color: The :class:`~viren2d.Color` used to initialize the
            output mask before rendering the gradient.

        Returns:
          A single-channel :class:`~viren2d.ImageBuffer` of type
          :class:`numpy.float64` with values :math:`\in [0, 1]` as the
          result of rendering the given gradient.

        Example:
          >>> grad = viren2d.LinearColorGradient((0, 0), (200, 200))
          >>> grad.add_color_stop(0.1, 1.0)
          >>> grad.add_color_stop(0.5, 0.5)
          >>> grad.add_color_stop(0.9, 1.0)
          >>> mask = viren2d.color_gradient_mask(
          >>>   grad, width=200, height=200, fill_color="black!0")
        )docstr",
        py::arg("gradient"),
        py::arg("width"),
        py::arg("height"),
        py::arg("fill_color") = Color::Black.WithAlpha(0.0));
}

} // namespace bindings
} // namespace viren2d

