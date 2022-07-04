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
        "Autumn",
        ColorMap::Autumn,
        "Red-yellow color map.")
      .value(
        "Bone",
        ColorMap::Bone,
        "Black-blue-white color map.")
      .value(
        "Cold",
        ColorMap::Cold,
        "Black-blue-cyan-white color map.")
      .value(
        "Disparity",
        ColorMap::Disparity,
        "High contrast color map with subtle gradient discontinuities, suitable"
        "for depth/disparity images.")
      .value(
        "Earth",
        ColorMap::Earth,
        "Black-green-white color map. Has linear grayscale changes when printed "
        "in black & white.")
      .value(
        "Grayscale",
        ColorMap::Grayscale,
        "Standard linear grayscale gradient color map.")
      .value(
        "Hot",
        ColorMap::Hot,
        "Black-red-yellow-white color map.")
      .value(
        "HSV",
        ColorMap::HSV,
        "Red-yellow-green-cyan-blue-magenta-red color map.")
      .value(
        "Inferno",
        ColorMap::Inferno,
        "Black-purple-red-yellow, perceptually uniform color map with "
        "monotonically increasing luminance.")
      .value(
        "Jet",
        ColorMap::Jet,
        "Rainbow color map - note that despite their good contrast, "
        "`these maps should be avoided! <http://jakevdp.github.io/blog/2014/10/16/how-bad-is-your-colormap/>`__!")
      .value(
        "Magma",
        ColorMap::Magma,
        "Perceptually uniform color map, similar to :attr:`~viren2d.ColorMap.Inferno`.")
      .value(
        "Pastel",
        ColorMap::Pastel,
        "Black-pastel-white color map. Has linear grayscale changes when printed in black & white.")
      .value(
        "Plasma",
        ColorMap::Plasma,
        "Perceptually uniform color map, similar to :attr:`~viren2d.ColorMap.Inferno`, "
        "but avoids black.")
      .value(
        "Sepia",
        ColorMap::Sepia,
        "Black-brown-white color map, perceptually uniform.")
      .value(
        "Temperature",
        ColorMap::Temperature,
        "Blue-pale-dark red color map, for visualizing data related to "
        "temperature. Has good contrast for colorblind viewers.")
      .value(
        "Thermal",
        ColorMap::Thermal,
        "Black-purple-red-yellow-white color map.")
      .value(
        "Turbo",
        ColorMap::Turbo,
        "An improved rainbow color map, "
        "`similar to (but smoother than) Jet <https://ai.googleblog.com/2019/08/turbo-improved-rainbow-colormap-for.html>`__.")
      .value(
        "Viridis",
        ColorMap::Viridis,
        "Perceptually uniform. Default color map of `matplotlib <https://matplotlib.org/>`__.");


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

  py::enum_<Colorizer::LimitsMode> mode(m, "ColorMap",
             "TODO DOC");

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
              visualization **for each new image**.
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

