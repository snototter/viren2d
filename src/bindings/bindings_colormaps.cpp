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
  py::enum_<ColorMap> cm(m, "ColorMap", R"docstr(
            Enumeration of available color maps.

            **Corresponding C++ API:** ``viren2d::ColorMap``.
            )docstr");
  cm.value(
        "Autumn",
        ColorMap::Autumn, R"docstr(
        Sequential red-to-yellow color map. This color map is not perceptually
        uniform. Based on `matplotlib's <https://matplotlib.org>`__ *autumn*
        map.
        )docstr")
      .value(
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
        "Categories12",
        ColorMap::Categories12, R"docstr(
        Color map with 12 distinct colors, suitable for categorical data. Based
        on `ColorBrewer2's <https://colorbrewer2.org>`__ *12-class paired* map.
        )docstr")
      .value(
        "Categories20",
        ColorMap::Categories20, R"docstr(
        Color map with 20 distinct colors, suitable for categorical data. Based
        on `matplotlib's <https://matplotlib.org>`__ *tab20* map.
        )docstr")
      .value(
        "Cividis",
        ColorMap::Cividis, R"docstr(
        Perceptually uniform sequential color map suitable for viewers with
        color vision deficiency (CVD). Published by
        `Nuñez, Anderton and Renslow <https://doi.org/10.1371/journal.pone.0199239>`__.
        )docstr")
      .value(
        "Cold",
        ColorMap::Cold, R"docstr(
        Blue shades from dark to light. This is the CET-L06 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "ColorBlindDiverging",
        ColorMap::ColorBlindDiverging, R"docstr(
        Perceptually uniform diverging color map for Protanopic/Deuteranopic
        viewers. This is the CET-CBD1 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "ColorBlindSequential",
        ColorMap::ColorBlindSequential, R"docstr(
        Perceptually uniform sequential color map for Protanopic/Deuteranopic
        viewers. This is the CET-CBL1 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "ColorBlindSequentialVivid",
        ColorMap::ColorBlindSequentialVivid, R"docstr(
        Perceptually uniform sequential color map with maximal chroma, suitable
        for Protanopic/Deuteranopic viewers. This is the CET-CBL2 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "ColorBlindOrientation",
        ColorMap::ColorBlindOrientation, R"docstr(
        Cyclic blue-white-yellow-black color map for four orientations/phase
        angles, suitable for Protanopic/Deuteranopic viewers.
        This is the CET-CBC1 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "Copper",
        ColorMap::Copper, R"docstr(
        Sequential color map from black to light copper. This color map has
        kinks in the lightness curve, which can lead to a perceived banding of
        the data in those value ranges. Based on
        `matplotlib's <https://matplotlib.org>`__ *copper* map.
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
        *i.e.* light colors are omitted. This color map is adapted from the
        `colorcet Python package <https://github.com/holoviz/colorcet>`__
        and was initially created using
        `Glasbey's method <https://doi.org/10.1002/col.20327>`__.
        )docstr")
      .value(
        "GlasbeyLight",
        ColorMap::GlasbeyLight, R"docstr(
        Color map for categorical data, best suited for **dark backgrounds**,
        *i.e.* dark colors are omitted. This color map is adapted from the
        `colorcet Python package <https://github.com/holoviz/colorcet>`__
        and was initially created using
        `Glasbey's method <https://doi.org/10.1002/col.20327>`__.
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
        ColorMap::Gray, R"docstr(
        Standard grayscale from black-to-white.
        )docstr")
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
        ColorMap::HSV, R"docstr(
        Cyclic color map obtained by varying the hue. Cycles through
        red-yellow-green-cyan-blue-magenta-red.
        )docstr")
      .value(
        "Inferno",
        ColorMap::Inferno, R"docstr(
        Bluish-to-reddish, perceptually uniform sequential color map. Proposed by
        `Stéfan van der Walt and Nathaniel Smith <https://bids.github.io/colormap/>`__
        and integrated into `matplotlib >= 1.15 <https://matplotlib.org/>`__.
        )docstr")
      .value(
        "Jet",
        ColorMap::Jet, R"docstr(
        The classic rainbow color map, based on
        `MATLAB's <https://www.mathworks.com/help/matlab/ref/jet.html>`__
        *jet* map.

        Note that this color map has several known limitations (*e.g.* reversed
        lightness gradients at yellow and red affect the viewer's perceptual
        ordering). Refer to the excellent article by
        `Peter Kovesi <https://arxiv.org/abs/1509.03700>`__ for more details
        about these issues.
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
        "Orientation4",
        ColorMap::Orientation4, R"docstr(
        Cyclic magenta-yellow-green-blue color map for four orientations/phase
        angles to be visualized. This is the CET-C2 color map by
        `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
        under the CC-BY 4.0 license.
        )docstr")
      .value(
        "Orientation6",
        ColorMap::Orientation6, R"docstr(
        Six-color cyclic map with primaries and secondaries matched in
        lightness. This is the CET-C6 color map by
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
        "Seismic",
        ColorMap::Seismic, R"docstr(
        Diverging blue-white-red map with vivid colors. Based on
        `matplotlib's <https://matplotlib.org>`__ *seismic* map.
        )docstr")
      .value(
        "Spectral",
        ColorMap::Spectral, R"docstr(
        Color map covering the spectral colors from ultra-violett to infrared.
        Based on `matplotlib's <https://matplotlib.org>`__ *NIPY Spectral* map.
        )docstr")
      .value(
        "SpectralDiverging",
        ColorMap::SpectralDiverging, R"docstr(
        Diverging redish-yellow-bluish map. Based on
        `matplotlib's <https://matplotlib.org>`__ *Spectral* map.
        )docstr")
      .value(
        "Spring",
        ColorMap::Spring, R"docstr(
        Sequential pink-to-yellow color map. This color map is not perceptually
        uniform. Based on `matplotlib's <https://matplotlib.org>`__ *spring*
        map.
        )docstr")
      .value(
        "Summer",
        ColorMap::Summer, R"docstr(
        Sequential green-to-yellow color map. This color map is not perceptually
        uniform. Based on `matplotlib's <https://matplotlib.org>`__ *summer*
        map.
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
        under the Apache-2.0 license. Often used for depth and disparity values.
        )docstr")
      .value(
        "Twilight",
        ColorMap::Twilight, R"docstr(
        Cyclic white-blue-black-red-white map with perceptually uniform lightness
        and color contrast over the whole range. Based on
        `matplotlib's <https://matplotlib.org>`__ *twilight* map.
        )docstr")
      .value(
        "TwilightShifted",
        ColorMap::TwilightShifted, R"docstr(
        Cyclic black-blue-white-red-black map with perceptually uniform lightness
        and color contrast over the whole range. Based on
        `matplotlib's <https://matplotlib.org>`__ *twilight_shifted* map.
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
        "Winter",
        ColorMap::Winter, R"docstr(
        Sequential blue-to-light-green color map. This color map is not
        perceptually uniform. Based on
        `matplotlib's <https://matplotlib.org>`__ *winter* map.
        )docstr")
      .value(
        "Yarg",
        ColorMap::Yarg, R"docstr(
        Inverted grayscale from white-to-black.
        )docstr");


  cm.def(
        "__str__", [](ColorMap c) -> py::str {
            return py::str(ColorMapToString(c));
        }, py::name("__str__"), py::is_method(m));


  cm.def(
        "__repr__", [](ColorMap c) -> py::str {
            std::ostringstream s;
            s << "<ColorMap \"" << ColorMapToString(c) << "\">";
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
    str << "Cannot cast type `" << tp << "` to `viren2d.ColorMap`!";
    throw std::invalid_argument(str.str());
  }
}


StreamColorizer::LimitsMode LimitsModeFromPyObject(const py::object &o) {
  if (py::isinstance<py::str>(o)) {
    return LimitsModeFromString(py::cast<std::string>(o));
  } else if (py::isinstance<StreamColorizer::LimitsMode>(o)) {
    return py::cast<StreamColorizer::LimitsMode>(o);
  } else {
    std::string s("Cannot cast type `");
    s += py::cast<std::string>(o.attr("__class__").attr("__name__"));
    s += "` to `viren2d.LimitsMode`!";
    throw std::invalid_argument(s);
  }
}


ImageBuffer ColorizationScaledHelper(
    const ImageBuffer &data, const py::object &colormap,
    double limit_low, double limit_high, int output_channels,
    int bins) {
  ColorMap cmap = ColorMapFromPyObject(colormap);
  return ColorizeScaled(data, cmap, limit_low, limit_high, output_channels, bins);
}


ImageBuffer ColorizationLabelsHelper(
    const ImageBuffer &labels, const py::object &colormap,
    int output_channels) {
  ColorMap cmap = ColorMapFromPyObject(colormap);
  return ColorizeLabels(labels, cmap, output_channels);
}


StreamColorizer CreateStreamColorizer(
    const py::object &colormap, const py::object &limits_mode, int bins,
    int output_channels, double low, double high) {
  ColorMap cm = ColorMapFromPyObject(colormap);
  StreamColorizer::LimitsMode lm = LimitsModeFromPyObject(limits_mode);
  return StreamColorizer(cm, lm, bins, output_channels, low, high);
}


void RegisterColormaps(pybind11::module &m) {
  py::enum_<StreamColorizer::LimitsMode> mode(m, "LimitsMode",
             "Enumeration of how the colorization limits should be computed.");
  mode.value(
        "Continuous",
        StreamColorizer::LimitsMode::FromDataContinuously, R"docstr(
        Compute limits for each incoming sample separately.

        Corresponding string representation: ``'continuous'``.
        )docstr")
      .value(
        "Fixed",
        StreamColorizer::LimitsMode::Fixed, R"docstr(
        Use fixed, user-defined limits.

        Corresponding string representation: ``'fixed'``.
        )docstr")
      .value(
        "Once",
        StreamColorizer::LimitsMode::FromDataOnce, R"docstr(
        Compute limits from the first incoming sample, then apply these to all
        further samples.

        Corresponding string representation: ``'continuous'``.
        )docstr");


  mode.def(
        "__str__", [](StreamColorizer::LimitsMode lm) -> py::str {
            return py::str(LimitsModeToString(lm));
        }, py::name("__str__"), py::is_method(m));


  mode.def(
        "__repr__", [](StreamColorizer::LimitsMode lm) -> py::str {
            std::ostringstream s;
            s << "<LimitsMode." << LimitsModeToString(lm) << '>';
            return py::str(s.str());
        }, py::name("__repr__"), py::is_method(m));



  py::class_<StreamColorizer> colorizer(m, "StreamColorizer", R"docstr(
      Utility class to simplify colorization of a data stream.

      This class takes care of computing/storing the input data limits, the
      selected color map, *etc.* This comes in handy whenever we need to apply
      the same colorization over and over again. For example, think of
      displaying the live stream of a time-of-flight sensor.

      **Corresponding C++ API:** ``viren2d::StreamColorizer``.

      Example:
        >>> depth_cam = ...  # Open camera stream
        >>> colorizer = viren2d.StreamColorizer(
        >>>     colormap=...)
        >>> while depth_cam.is_available():
        >>>     depth = depth_cam.next()
        >>>     vis = colorizer(depth)
      )docstr");


  colorizer.def(
        py::init<>(&CreateStreamColorizer), R"docstr(
        Creates a customized stream colorizer.

        Args:
          colormap: The :class:`~viren2d.ColorMap` to be used for
            colorization. In addition to the enumeration value, its
            string representation can be used for convenience.
          mode: The :class:`~viren2d.LimitsMode` specifying how the data limits
            should be computed. Can be provided as enumeration value or its
            corresponding string representation.

            If set to :attr:`LimitsMode.Fixed`, then the parameters
            ``low`` and ``high`` *must* be set to valid numbers.
          bins: Number of discretization bins as :class:`int`.
            Must be :math:`\geq 2`. This parameter will be ignored if the
            selected color map has less than ``bins`` colors.
          output_channels: Number of output channels as :class:`int`.
            Must be either 3 or 4. The optional 4th channel will be
            considered an alpha channel and set to 255.
          low: Lower limit of the input data. Will be considered only if
            ``mode`` is :attr:`LimitsMode.Fixed`.
          high: Upper limit of the input data. Will be considered only if
            ``mode`` is :attr:`LimitsMode.Fixed`.
        )docstr",
        py::arg("colormap"),
        py::arg("mode") = "continuous",
        py::arg("bins") = 256,
        py::arg("output_channels") = 3,
        py::arg("low") = std::numeric_limits<double>::infinity(),
        py::arg("high") = std::numeric_limits<double>::infinity())
      .def_property(
        "limit_low",
        &StreamColorizer::GetLimitLow,
        &StreamColorizer::SetLimitLow, R"docstr(
        :class:`float`: Lower limit of the input data.

          If you intend to set this to *inf*/*nan*, ensure that
          :attr:`limits_mode` is not set to ``fixed``, or a :class:`ValueError`
          will be raised.

          **Corresponding C++ API:** ``viren2d::StreamColorizer::GetLimitLow/SetLimitLow``.
        )docstr")
      .def_property(
        "limit_high",
        &StreamColorizer::GetLimitHigh,
        &StreamColorizer::SetLimitHigh, R"docstr(
        :class:`float`: Lower limit of the input data.

          If you intend to set this to *inf*/*nan*, ensure that
          :attr:`limits_mode` is not set to ``fixed``, or a :class:`ValueError`
          will be raised.

          **Corresponding C++ API:** ``viren2d::StreamColorizer::GetLimitHigh/SetLimitHigh``.
        )docstr")
      .def_property(
        "limits_mode",
        &StreamColorizer::GetLimitsMode,
        [](StreamColorizer &c, py::object lm) {
            c.SetLimitsMode(LimitsModeFromPyObject(lm));
        }, R"docstr(
        :class:`~viren2d.LimitsMode`: Specifies how the data limits
          should be computed. Can be set via the enumeration value or its
          corresponding string representation.

          Note that :attr:`limit_low` and :attr:`limit_high` must be set to
          valid numbers **before** the mode is changed to ``fixed``.

          **Corresponding C++ API:** ``viren2d::StreamColorizer::GetLimitsMode/SetLimitsMode``.
        )docstr")
      .def_property(
        "output_channels",
        &StreamColorizer::GetOutputChannels,
        &StreamColorizer::SetOutputChannels, R"docstr(
        :class:`int`: Number of output channels.

          Must be either 3 or 4. The optional 4th channel will be considered an
          alpha channel and set to 255.

          **Corresponding C++ API:** ``viren2d::StreamColorizer::GetOutputChannels/SetOutputChannels``.
        )docstr")
      .def_property(
        "bins",
        &StreamColorizer::GetBins,
        &StreamColorizer::SetBins, R"docstr(
        :class:`int`: Number of discretization bins.

          Must be :math:`\geq 2`. This parameter will be ignored if the
          selected :attr:`colormap` has less than ``bins`` colors.

          **Corresponding C++ API:** ``viren2d::StreamColorizer::GetBins/SetBins``.
        )docstr")
      .def_property(
        "colormap",
        &StreamColorizer::GetColorMap,
        [](StreamColorizer &c, const py::object &o) {
          c.SetColorMap(ColorMapFromPyObject(o));
        }, R"docstr(
        :class:`~viren2d.ColorMap`: The selected color map.

          In addition to the enumeration value, the corresponding string
          representation can be used to set this property.

          **Corresponding C++ API:** ``viren2d::StreamColorizer::GetColorMap/SetColorMap``.
        )docstr");


  m.def("colorize_scaled",
        &ColorizationScaledHelper, R"docstr(
        Colorizes 2D data array using a colormap.

        **Corresponding C++ API:** ``viren2d::ColorizeScaled``.

        Args:
          data: A single channel :class:`~viren2d.ImageBuffer` or
            :class:`numpy.ndarray` holding the data for colorization.
          colormap: The :class:`~viren2d.ColorMap` to be used for
            colorization. In addition to the enumeration value, its
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
          >>> vis = viren2d.colorize_scaled(
          >>>     data, colormap='gouldian', low=-8, high=8,
          >>>     bins=256, output_channels=3)
        )docstr",
        py::arg("data"),
        py::arg("colormap") = ColorMap::Gouldian,
        py::arg("low") = std::numeric_limits<double>::infinity(),
        py::arg("high") = std::numeric_limits<double>::infinity(),
        py::arg("output_channels") = 3,
        py::arg("bins") = 256);


  m.def("colorize_labels",
        &ColorizationLabelsHelper, R"docstr(
        Colorizes a label image.

        **Corresponding C++ API:** ``viren2d::ColorizeLabels``.

        Args:
          labels: A single channel :class:`~viren2d.ImageBuffer` or
            :class:`numpy.ndarray` holding the labels as integral data type.
          colormap: The :class:`~viren2d.ColorMap` to be used for
            colorization. In addition to the enumeration value, its
            string representation can be used for convenience.
          output_channels: Number of output channels as :class:`int`.
            Must be either 3 or 4. The optional 4th channel will be
            considered an alpha channel and set to 255.

        Returns:
          A 3- or 4-channel :class:`~viren2d.ImageBuffer` of
          type :class:`numpy.uint8`.

        Example:
          >>> import numpy as np
          >>> labels = np.array(
          >>>     [[1, 2, 3], [4, 5, 6], [20000, 20001, 20003]],
          >>>     dtype=np.int32)
          >>> vis = viren2d.colorize_labels(
          >>>     labels, colormap='category-20', output_channels=3)
        )docstr",
        py::arg("labels"),
        py::arg("colormap") = ColorMap::GlasbeyDark,
        py::arg("output_channels") = 3);


  m.def("relief_shading",
        &ReliefShading, R"docstr(
        Multiplicative relief shading.

        Combines a false color representation with the relief data via
        element-wise multiplication. This can enhance the shape perception
        significantly due to the shading.

        Note that the :class:`~viren2d.ColorMap` should be selected with care
        to avoid interfering with the perception of features induced by the
        shading. Particularly well suited color maps have only low (or even no)
        variation in lightness values, *e.g.* :attr:`ColorMap.Relief` or
        :attr:`ColorMap.ReliefLowContrast`.

        **Corresponding C++ API:** ``viren2d::ReliefShading``.

        Args:
          relief: A single channel :class:`~viren2d.ImageBuffer` or
            :class:`numpy.ndarray` holding the topographic data. If the data
            is an integer type, it will be divided by 255 and converted
            to single precision floating point before multiplication.

          colorized: An :class:`~viren2d.ImageBuffer` or
            :class:`numpy.ndarray` of type :class:`numpy.uint8` holding the
            color image to be shaded.

        Returns:
          An :class:`~viren2d.ImageBuffer` of type :class:`numpy.uint8` which
          has the same number of channels as ``colorized``.

        Example:
          >>> moon = viren2d.load_image_uint8('examples/data/lunar-farside.jpg')
          >>> vis = viren2d.colorize_scaled(
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

        **Corresponding C++ API:** ``viren2d::Peaks``.

        Returns:
          A ``width`` by ``height`` single-channel :class:`~viren2d.ImageBuffer`
          of type :class:`numpy.float64`.
        )docstr",
        py::arg("height") = 600,
        py::arg("width") = 600);
}
} // namespace bindings
} // namespace viren2d

