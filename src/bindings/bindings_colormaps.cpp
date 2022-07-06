#include <sstream>
#include <stdexcept>
#include <limits>

#include <pybind11/stl.h>

#include <bindings/binding_helpers.h>

#include <viren2d/colormaps.h>

namespace py = pybind11;

namespace viren2d {
namespace bindings {
void RegisterColorMapEnum(pybind11::module &m) {
  py::enum_<ColorMap> cm(m, "ColorMap",
             "Available color maps.");
  cm.value(
        "BlackBody",
        ColorMap::BlackBody, R"docstr(
        Black-red-yellow-white, perceptually uniform sequential color map
        inspired by black-body radiation. This color map definition has
        been taken from
        `Kenneth Moreland's website <https://www.kennethmoreland.com/color-advice/>`__.
        )docstr")
      .value(
        "Cold",
        ColorMap::Cold, R"docstr(
        Blue shades from dark to light. This is the CET-L06 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "ColorBlind",
        ColorMap::ColorBlind, R"docstr(
        Perceptually uniform sequential color map for Protanopic/Deuteranopic
        viewers. This is the CET-CBL1 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "Disparity",
        ColorMap::Disparity, R"docstr(
        High contrast color map for depth & disparity images.
        Based on `disparity` for MATLAB
        `by Oliver Woodford <https://github.com/ojwoodford/sc>`__,
        who released it under the BSD 3-Clause license.
        )docstr")
      .value(
        "GlasbeyDark",
        ColorMap::GlasbeyDark, R"docstr(
        Color map for categorical data, best suited for **light backgrounds**,
        *i.e.* light colors are omitted. This color map is adapted from
        `colorcet <https://github.com/holoviz/colorcet>`__ and was created using
        `Glasbey's method <https://strathprints.strath.ac.uk/30312/1/colorpaper_2006.pdf>`.
        TODO mention usage with updated color::byid, etc.
        )docstr")
      .value(
        "GlasbeyLight",
        ColorMap::GlasbeyLight, R"docstr(
        Color map for categorical data, best suited for **dark backgrounds**,
        *i.e.* dark colors are omitted. This color map is adapted from
        `colorcet <https://github.com/holoviz/colorcet>`__ and was created using
        `Glasbey's method <https://strathprints.strath.ac.uk/30312/1/colorpaper_2006.pdf>`.
        )docstr")
      .value(
        "Gouldian",
        ColorMap::Gouldian, R"docstr(
        Black-blue-green-orange-yellow perceptually uniform sequential color
        map. Similar to MATLAB's `parula`, but with a smoother path and more
        uniform slope upwards in CIELAB space. This is the CET-L20 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "Gray",
        ColorMap::Gray,
        "Standard grayscale from black-to-white.")
      .value(
        "Hell",
        ColorMap::Hell, R"docstr(
        Black-purple-red-yellow-white, perceptually uniform sequential color
        map. Similar to `Inferno` but starts from black and ends white.
        Based on `hell` from `Agama <https://github.com/GalacticDynamics-Oxford/Agama>`__.
        )docstr")
      .value(
        "Hot",
        ColorMap::Hot, R"docstr(
        Black-red-yellow-white perceptually uniform sequential color map.
        This is the CET-L03 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "HSV",
        ColorMap::HSV,
        "Cyclic color map by varying the hue: Red-yellow-green-cyan-blue-magenta-red.")
      .value(
        "Inferno",
        ColorMap::Inferno, R"docstr(
        Bluish-to-reddish, perceptually uniform sequential color map. Proposed by
        `Stéfan van der Walt and Nathaniel Smith <https://bids.github.io/colormap/>`__
        and integrated into `matplotlib >= 1.15 <https://matplotlib.org/>`__.
        )docstr")
      .value(
        "Orientation",
        ColorMap::Orientation, R"docstr(
        Cyclic magenta-yellow-green-blue color map for four orientations/phase
        angles to be visualized. This is the CET-C2 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "OrientationColorBlind",
        ColorMap::OrientationColorBlind, R"docstr(
        Cyclic blue-white-yellow-black color map for four orientations/phase
        angles, suitable for Protanopic/Deuteranopic viewers.
        This is the CET-CBC1 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "Rainbow",
        ColorMap::Rainbow, R"docstr(
        The "least worse" rainbow color map, i.e. CET-R2, by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "Relief",
        ColorMap::Relief, R"docstr(
        Green-brownish-bluish perceptually uniform sequential color map for
        relief shading. This is the CET-L11 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "ReliefIsoluminant",
        ColorMap::ReliefIsoluminant, R"docstr(
        Low contrast, blue-green-orange perceptually uniform sequential color map
        for relief shading. This is the CET-I2 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "Temperature",
        ColorMap::Temperature, R"docstr(
        Diverging blue-white-red color map.
        This is the CET-D01A color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "TemperatureDark",
        ColorMap::TemperatureDark, R"docstr(
        Diverging blue-black-red color map.
        This is the CET-D04 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "Thermal",
        ColorMap::Thermal, R"docstr(
        Black-purple-red-yellow-white sequential color map for
        thermographic images, also known as *iron* or *ironbow*.
        )docstr")
      .value(
        "Turbo",
        ColorMap::Turbo, R"docstr(
        A rainbow color map similar to the well-known `jet`, but following a
        smoother path through the CIELAB color space. Published by
        `Google (Anton Mikhailov) <https://ai.googleblog.com/2019/08/turbo-improved-rainbow-colormap-for.html>`__
        under the Apache-2.0 license.
        )docstr")
      .value(
        "Viridis",
        ColorMap::Viridis, R"docstr(
        Perceptually uniform sequential color map. Proposed by
        `Stéfan van der Walt and Nathaniel Smith <https://bids.github.io/colormap/>`__,
        now the default color map of `matplotlib <https://matplotlib.org/>`__.
        )docstr")
      .value(
        "Water",
        ColorMap::Water, R"docstr(
        Perceptually uniform sequential color map for water depth.
        This is the CET-L12 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "Yarg",
        ColorMap::Yarg,
        "Inverted grayscale from white-to-black.");


  cm.def(
        "__str__", [](ColorMap c) -> py::str {
            return py::str(ColorMapToString(c));
        }, py::name("__str__"), py::is_method(m));

  cm.def(
        "__repr__", [](ColorMap c) -> py::str {
            std::ostringstream s;
            s << "<Colormap." << ColorMapToString(c) << '>';
            return py::str(s.str());
        }, py::name("__repr__"), py::is_method(m));


  std::string doc = R"docstr(
      Returns all :class:`~viren2d.ColorMap` values.

      Convenience utility to easily iterate all enumeration
      values.

      **Corresponding C++ API:** ``viren2d::ListColorMaps``.
      )docstr";
  cm.def_static("list_all", &ListColorMaps, doc.c_str());

}


ColorMap ColorMapFromPyObject(const py::object &o) {
  if (py::isinstance<py::str>(o)) {
    return ColorMapFromString(py::cast<std::string>(o));
  } else if (py::isinstance<ColorMap>(o)) {
    return py::cast<ColorMap>(o);
  } else {
    const std::string tp = py::cast<std::string>(
        o.attr("__class__").attr("__name__"));
    std::ostringstream str;
    str << "Cannot cast type `" << tp
        << "` to `viren2d.ColorMap`!";
    throw std::invalid_argument(str.str());
  }
}


ImageBuffer ColorizationHelper(
    const ImageBuffer &data, const py::object &colormap,
    double limit_low, double limit_high, int output_channels,
    int bins) {
  ColorMap cmap = ColorMapFromPyObject(colormap);
  return Colorize(data, cmap, limit_low, limit_high, output_channels, bins);
}


Colorizer CreateColorizer(
    ColorMap color_map, const std::string &limits_mode, int bins,
    int output_channels, double low, double high) {
  Colorizer::LimitsMode lm = LimitsModeFromString(limits_mode);
  return Colorizer(color_map, lm, bins, output_channels, low, high);
}


void RegisterColormaps(pybind11::module &m) {
  RegisterColorMapEnum(m);

  py::enum_<Colorizer::LimitsMode> mode(m, "LimitsMode",
             "TODO DOC LimitsMode");

  //TODO bind mode!
  py::class_<Colorizer> colorizer(m, "Colorizer", R"docstr(
      TODO

      TODO useful in scenarios, where we need to apply the
      same colorization to similar data. For example, when
      displaying the live stream of a time-of-flight sensor.

      Example:
        >>> #TODO
      )docstr");

  colorizer.def(
        py::init<>(&CreateColorizer), R"docstr(
        TODO doc

        Args:
          color_map:
          mode: TODO as :class:`str`

            * ``'continuos'``: Computes the upper and lower limits for
              visualization **per image**.
            * ``'fixed'``: Provide upper and lower limits as ``low`` and ``high`` parameters.
            * ``'once'``: TODO
        )docstr",
        py::arg("color_map"),
        py::arg("mode") = "continuous",
        py::arg("bins") = 256,
        py::arg("output_channels") = 3,
        py::arg("low") = std::numeric_limits<double>::infinity(),
        py::arg("high") = std::numeric_limits<double>::infinity())
      .def_property(
        "limit_low", &Colorizer::LimitLow, &Colorizer::SetLimitLow,
        ":class:`float`: TODO doc")
      .def_property(
        "limit_high", &Colorizer::LimitHigh, &Colorizer::SetLimitHigh,
        ":class:`float`: TODO doc");


  m.def("colorize",
        &ColorizationHelper, R"docstr(
        Colorizes 2D data array using a colormap.

        Args:
          data: A single channel :class:`~viren2d.ImageBuffer` or
            :class:`numpy.ndarray` holding the data for colorization.
          colormap: The :class:`~viren2d.ColorMap` to be used for
            colorization. In addition to the enum value, the corresponding
            string representation can be used for convenience.
          low: Lower limit of the input values as :class:`float`.
          high: Upper limit of the input values as :class:`float`.
          output_channels: Number of output channels as :class:`int`.
            Must be either 3 or 4. The optional 4th channel will be
            considered an alpha channel and set to 255.
          bins: Number of discretization bins as :class:`int`.
            Must be :math:`2 \leq bins \leq 256`.

        Returns:
          A 3- or 4-channel :class:`~viren2d.ImageBuffer` of
          type :class:`numpy.uint8`.

        Example:
          >>> data = viren2d.peaks(400, 400)
          >>> vis = viren2d.colorize(
          >>>     data, colormap='viridis', low=-8, high=8, bins=256, output_channels=3)
        )docstr",
        py::arg("data"),
        py::arg("colormap") = ColorMap::Viridis,
        py::arg("low") = 0.0,
        py::arg("high") = 1.0,
        py::arg("output_channels") = 3,
        py::arg("bins") = 256);


  m.def("peaks",
        &Peaks, R"docstr(
        Computes the `peaks` example data.

        Computes exemplary data from translated and scaled Gaussian
        distributions, known from MATLAB's `peaks`. For details on
        the formal definition, refer to the
        `MATLAB documentation <https://www.mathworks.com/help/matlab/ref/peaks.html>`__.

        Returns:
          A ``width`` by ``height`` single-channel :class:`~viren2d.ImageBuffer`
          of type :class:`numpy.float64`.
        )docstr",
        py::arg("height") = 600,
        py::arg("width") = 600);
}
} // namespace bindings
} // namespace viren2d

