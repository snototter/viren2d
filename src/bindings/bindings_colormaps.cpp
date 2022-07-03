#include <sstream>
#include <stdexcept>

#include <pybind11/stl.h>

#include <bindings/binding_helpers.h>

#include <viren2d/colormaps.h>

namespace py = pybind11;

namespace viren2d {
namespace bindings {
void RegisterColormapEnum(pybind11::module &m) {
  py::enum_<Colormap> cm(m, "Colormap",
             "Available colormaps.");
  cm.value(
        "Autumn",
        Colormap::Autumn,
        "Red-yellow colormap.")
      .value(
        "Bone",
        Colormap::Bone,
        "Black-blue-white colormap.")
      .value(
        "Cold",
        Colormap::Cold,
        "Black-blue-cyan-white colormap.")
      .value(
        "Disparity",
        Colormap::Disparity,
        "High contrast colormap with subtle gradient discontinuities, suitable"
        "for depth/disparity images.")
      .value(
        "Earth",
        Colormap::Earth,
        "Black-green-white colormap. Has linear grayscale changes when printed "
        "in black & white.")
      .value(
        "Grayscale",
        Colormap::Grayscale,
        "Linear grayscale gradient.")
      .value(
        "Hot",
        Colormap::Hot,
        "Black-red-yellow-white colormap.")
      .value(
        "HSV",
        Colormap::HSV,
        "Red-yellow-green-cyan-blue-magenta-red colormap.")
      .value(
        "Inferno",
        Colormap::Inferno,
        "Black-purple-red-yellow, perceptually uniform colormap with "
        "monotonically increasing luminance.")
      .value(
        "Jet",
        Colormap::Jet,
        "Rainbow colormap - note that despite their good contrast, "
        "`these maps should be avoided! <http://jakevdp.github.io/blog/2014/10/16/how-bad-is-your-colormap/>`__!")
      .value(
        "Magma",
        Colormap::Magma,
        "Perceptually uniform colormap, similar to :attr:`~viren2d.ColorMap.Inferno`.")
      .value(
        "Pastel",
        Colormap::Pastel,
        "Black-pastel-white colormap. Has linear grayscale changes when printed in black & white.")
      .value(
        "Plasma",
        Colormap::Plasma,
        "Perceptually uniform colormap, similar to :attr:`~viren2d.ColorMap.Inferno`, "
        "but avoids black.")
      .value(
        "Sepia",
        Colormap::Sepia,
        "Black-brown-white colormap, perceptually uniform.")
      .value(
        "Temperature",
        Colormap::Temperature,
        "Blue-pale-dark red colormap, for visualizing data related to "
        "temperature. Has good contrast for colorblind viewers.")
      .value(
        "Thermal",
        Colormap::Thermal,
        "Black-purple-red-yellow-white colormap.")
      .value(
        "Turbo",
        Colormap::Turbo,
        "An improved rainbow colormap, "
        "`similar to (but smoother than) Jet <https://ai.googleblog.com/2019/08/turbo-improved-rainbow-colormap-for.html>`__.")
      .value(
        "Viridis",
        Colormap::Viridis,
        "Perceptually uniform. Default colormap of `matplotlib <https://matplotlib.org/>`__.");


  cm.def(
        "__str__", [](Colormap c) -> py::str {
            return py::str(ColormapToString(c));
        }, py::name("__str__"), py::is_method(m));

  cm.def(
        "__repr__", [](Colormap c) -> py::str {
            std::ostringstream s;
            s << "<Colormap." << ColormapToString(c) << '>';
            return py::str(s.str());
        }, py::name("__repr__"), py::is_method(m));
}


Colormap ColormapFromPyObject(const py::object &o) {
  if (py::isinstance<py::str>(o)) {
    return ColormapFromString(py::cast<std::string>(o));
  } else if (py::isinstance<Colormap>(o)) {
    return py::cast<Colormap>(o);
  } else {
    const std::string tp = py::cast<std::string>(
        o.attr("__class__").attr("__name__"));
    std::ostringstream str;
    str << "Cannot cast type `" << tp
        << "` to `viren2d.Colormap`!";
    throw std::invalid_argument(str.str());
  }
}


ImageBuffer ColorizationHelper(
    const ImageBuffer &data, const py::object &colormap,
    double limit_low, double limit_high, int output_channels,
    int bins) {
  Colormap cmap = ColormapFromPyObject(colormap);
  return Colorize(data, cmap, limit_low, limit_high, output_channels, bins);
}


void RegisterColormaps(pybind11::module &m) {
  RegisterColormapEnum(m);

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
        py::arg("colormap") = Colormap::Viridis,
        py::arg("low") = 0.0,
        py::arg("high") = 1.0,
        py::arg("output_channels") = 3,
        py::arg("bins") = 256);

  m.def("colormap_names", &ListColormaps, "TODO DOC corresponding cpp api...");

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

