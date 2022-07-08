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
             "Enum to select a color map.");
  cm.value(
        "BlackBody",
        ColorMap::BlackBody, R"docstr(
        Black-red-yellow-white, perceptually uniform sequential color map
        inspired by black-body radiation. This color map definition has
        been taken from
        `Kenneth Moreland's website <https://www.kennethmoreland.com/color-advice/>`__.
        )docstr")
      .value(
        "Categories10",
        ColorMap::Categories10, R"docstr(
        Color map with 10 distinct colors, suitable for categorical data. Based
        on `matplotlib's <https://matplotlib.org>`__ *tab10* map.
        )docstr")
      .value(
        "Categories20",
        ColorMap::Categories20, R"docstr(
        Color map with 20 distinct colors, suitable for categorical data. Based
        on `matplotlib's <https://matplotlib.org>`__ *tab20* map.
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
        "Earth",
        ColorMap::Earth, R"docstr(
        A blue-green-brown color map to visualize topography data. Based on
        `matplotlib's <https://matplotlib.org>`__ *gist_earth* map.
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
        "Ocean",
        ColorMap::Ocean, R"docstr(
        A green-blue color map to visualize water depths. Based on
        `matplotlib's <https://matplotlib.org>`__ *ocean* map.
        )docstr")
      .value(
        "OpticalFlow",
        ColorMap::OpticalFlow, R"docstr(
        Cyclic HSV-like color map with reduced colors. Suitable to visualize
        optical flow fields. Based on the color map used by the
        `Middlebury Optical Flow benchmark <https://vision.middlebury.edu/flow/>`__.
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
        "ReliefLowContrast",
        ColorMap::ReliefLowContrast, R"docstr(
        Low contrast (isoluminant), blue-green-orange perceptually uniform
        sequential color map for relief shading. This is the CET-I2 color map by
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
        "Terrain",
        ColorMap::Terrain, R"docstr(
        A blue-green-yellowish-brownish color map to visualize topography
        data. Based on `matplotlib's <https://matplotlib.org>`__ *terrain* map.
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
        A rainbow color map similar to the well-known MATLAB `jet`, but following
        a smoother path through the CIELAB color space. Published by
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


Colorizer::LimitsMode LimitsModeFromPyObject(const py::object &o) {
  if (py::isinstance<py::str>(o)) {
    return LimitsModeFromString(py::cast<std::string>(o));
  } else if (py::isinstance<Colorizer::LimitsMode>(o)) {
    return py::cast<Colorizer::LimitsMode>(o);
  } else {
    std::string s("Cannot cast type `");
    s += py::cast<std::string>(
          o.attr("__class__").attr("__name__"));
    s += "` to `viren2d.LimitsMode`!";
    throw std::invalid_argument(s);
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
    ColorMap colormap, const py::object &limits_mode, int bins,
    int output_channels, double low, double high) {
  Colorizer::LimitsMode lm = LimitsModeFromPyObject(limits_mode);
  return Colorizer(colormap, lm, bins, output_channels, low, high);
}


void RegisterColormaps(pybind11::module &m) {
  RegisterColorMapEnum(m);

  py::enum_<Colorizer::LimitsMode> mode(m, "LimitsMode",
             "Specifies how the colorization limits should be computed.");

  //TODO bind mode!
  py::class_<Colorizer> colorizer(m, "Colorizer", R"docstr(
      Utility class to simplify colorization of a data stream.

      This class takes care of computing/storing the limits, color map,
      *etc.* This becomes handy in scenarios where we need to apply the same
      colorization over and over again. For example, when displaying the live
      stream of a time-of-flight sensor.

      Example:
        >>> #TODO
        >>> depth_cam = ...
        >>> colorizer = viren2d.Colorizer(
        >>>     colormap=...)
        >>> while depth_cam.is_available():
        >>>     depth = depth_cam.next()
        >>>     vis = colorizer(depth)
      )docstr");

  colorizer.def(
        py::init<>(&CreateColorizer), R"docstr(
        TODO doc

        Args:
          colormap:
          mode: TODO as :class:`~viren2d.LimitsMode` or :class:`str`

            * ``'continuous'``: Computes the upper and lower limits for
              visualization **per image**.
            * ``'fixed'``: Provide upper and lower limits as ``low`` and ``high`` parameters.
            * ``'once'``: TODO
        )docstr",
        py::arg("colormap"),
        py::arg("mode") = "continuous",
        py::arg("bins") = 256,
        py::arg("output_channels") = 3,
        py::arg("low") = std::numeric_limits<double>::infinity(),
        py::arg("high") = std::numeric_limits<double>::infinity())
      .def_property(
        "limit_low",
        &Colorizer::LimitLow,
        &Colorizer::SetLimitLow, R"docstr(
        :class:`float`: Lower limit of the input data.

          If you intend to set this to *inf*/*nan*, ensure that
          :attr:`limits_mode` is not set to ``fixed``, or a :class:`ValueError`
          will be raised.
        )docstr")
      .def_property(
        "limit_high",
        &Colorizer::LimitHigh,
        &Colorizer::SetLimitHigh, R"docstr(
        :class:`float`: Lower limit of the input data.

          If you intend to set this to *inf*/*nan*, ensure that
          :attr:`limits_mode` is not set to ``fixed``, or a :class:`ValueError`
          will be raised.
        )docstr")
      .def_property(
        "output_channels",
        &Colorizer::OutputChannels,
        &Colorizer::SetOutputChannels,
        ":class:`int`: Number of output channels. Must be either 3 or 4.")
      .def_property(
        "colormap",
        &Colorizer::GetColorMap,
        [](Colorizer &c, const py::object &o) {
          c.SetColorMap(ColorMapFromPyObject(o));
        }, R"docstr(
        :class:`~viren2d.ColorMap`: TODO set via enum or string representation TODO doc
        )docstr");


  m.def("colorize",
        &ColorizationHelper, R"docstr(
        Colorizes 2D data array using a colormap.

        Args:
          data: A single channel :class:`~viren2d.ImageBuffer` or
            :class:`numpy.ndarray` holding the data for colorization.
          colormap: The :class:`~viren2d.ColorMap` to be used for
            colorization. In addition to the enum value, the corresponding
            string representation can be used for convenience.
          low: Lower limit of the input values as :class:`float`. If either
            ``low`` or ``high`` are ``inf`` or ``nan``, **both limits** will
            be computed from the input ``data``.
          high: Upper limit of the input values as :class:`float`.
          output_channels: Number of output channels as :class:`int`.
            Must be either 3 or 4. The optional 4th channel will be
            considered an alpha channel and set to 255.
          bins: Number of discretization bins as :class:`int`.
            Must be :math:`\geq 2`. This parameter will be ignored if the
            selected color map has less than ``bins`` colors.

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
        py::arg("low") = std::numeric_limits<double>::infinity(),
        py::arg("high") = std::numeric_limits<double>::infinity(),
        py::arg("output_channels") = 3,
        py::arg("bins") = 256);


  m.def("relief_shading",
        &ReliefShading, R"docstr(
        TODO doc multiplicative combination for relief shading.

        Args:
          relief: A single channel :class:`~viren2d.ImageBuffer` or
            :class:`numpy.ndarray` holding the topographic data.
            TODO doc implicit scaling if integral type!

          colorized: An :class:`~viren2d.ImageBuffer` or
            :class:`numpy.ndarray` of type :class:`numpy.uint8` holding the
            color image to be shaded.

        Returns:
          TODO

        Example:
          >>> moon = viren2d.load_image_uint8('examples/data/lunar-farside.jpg')
          >>> vis = viren2d.colorize(
          >>>     data=moon, colormap='relief-low-contrast', low=0, high=255)
          >>> shaded = viren2d.relief_shading(relief=moon, colorized=vis)
        )docstr",
        py::arg("relief"),
        py::arg("colorized"));

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

