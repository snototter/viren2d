#include <sstream>
#include <stdexcept>
#include <limits>

#include <pybind11/stl.h>

#include <bindings/binding_helpers.h>

#include <viren2d/colormaps.h>

namespace py = pybind11;

namespace viren2d {
namespace bindings {
std::string ColorMapCodeExampleScaled(ColorMap cmap) {
  std::ostringstream code_block;
  code_block << "\n\n    Example:\n"
    << "      >>> import numpy as np\n"
    << "      >>> import viren2d\n"
    << "      >>> data = np.vstack(25 * (np.arange(0, 256),))\n"
    << "      >>> vis = viren2d.colorize_scaled(\n"
    << "      >>>     data, colormap='" << ColorMapToString(cmap)
    << "', low=0, high=255,\n"
    << "      >>>     bins=256, output_channels=3)\n";
  return code_block.str();
}


std::string ColorMapCodeExampleCategorical(ColorMap cmap) {
  std::ostringstream code_block;
  code_block << "\n\n    Example:\n"
    << "      >>> import numpy as np\n"
    << "      >>> import viren2d\n"
    << "      >>> data = np.random.randint(0, 20000, (10, 20))\n"
    << "      >>> vis = viren2d.colorize_labels(\n"
    << "      >>>     data, colormap='" << ColorMapToString(cmap)
    << "', output_channels=3)\n";
  return code_block.str();
}


std::string ColorMapCodeExampleOrientation(ColorMap cmap) {
  std::ostringstream code_block;
  code_block << "\n\n    Example:\n"
    << "      >>> import viren2d\n"
    << "      >>> vis = viren2d.optical_flow_legend(size=200,\n"
    << "      >>>     colormap='" << ColorMapToString(cmap)
    << "', output_channels=4, clip_circle=True)\n";
  return code_block.str();
}


std::string ColorMapCodeExampleCustom(ColorMap cmap) {
  std::ostringstream code_block;
  code_block << "\n\n    Example:\n"
    << "      >>> import numpy as np\n"
    << "      >>> import viren2d\n"
    << "      >>> data = np.vstack(25 * (np.arange(0, 256),))\n"
    << "      >>> viren2d.set_custom_colormap(\n"
    << "      >>>     '" << ColorMapToString(cmap) << "',\n"
    << "      >>>     ['navy-blue', 'azure', 'blue', 'light-blue'])\n"
    << "      >>> vis = viren2d.colorize_scaled(\n"
    << "      >>>     data, colormap='" << ColorMapToString(cmap)
    << "', low=0, high=255,\n"
    << "      >>>     bins=256, output_channels=3)\n";
  return code_block.str();
}


void RegisterColorMapEnum(pybind11::module &m) {
  py::enum_<ColorMap> cm(m, "ColorMap", R"docstr(
            Enumeration of available color maps.

            **Corresponding C++ API:** ``viren2d::ColorMap``.
            )docstr");


  std::string docstr = R"docstr(
    Sequential red-to-yellow color map. This color map is not perceptually
    uniform. Based on `matplotlib's <https://matplotlib.org>`__ *autumn*
    map.

    .. image:: ../images/colormap-gradient-autumn.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Autumn);
  cm.value("Autumn", ColorMap::Autumn, docstr.c_str());


  docstr = R"docstr(
    Black-red-yellow-white, perceptually uniform sequential color map
    inspired by black-body radiation. This color map definition has
    been taken from
    `Kenneth Moreland's website <https://www.kennethmoreland.com/color-advice/#black-body>`__.

    .. image:: ../images/colormap-gradient-black-body.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::BlackBody);
  cm.value("BlackBody", ColorMap::BlackBody, docstr.c_str());


  docstr = R"docstr(
    Color map with 10 distinct colors, suitable for categorical data. Based
    on `matplotlib's <https://matplotlib.org>`__ *tab10* map.

    .. image:: ../images/colormap-gradient-categories-10.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleCategorical(ColorMap::Categories10);
  cm.value("Categories10", ColorMap::Categories10, docstr.c_str());


  docstr = R"docstr(
    Color map with 12 distinct colors, suitable for categorical data. Based
    on `ColorBrewer2's <https://colorbrewer2.org>`__ *12-class paired* map.

    .. image:: ../images/colormap-gradient-categories-12.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleCategorical(ColorMap::Categories12);
  cm.value("Categories12", ColorMap::Categories12, docstr.c_str());


  docstr = R"docstr(
    Color map with 20 distinct colors, suitable for categorical data. Based
    on `matplotlib's <https://matplotlib.org>`__ *tab20* map.

    .. image:: ../images/colormap-gradient-categories-20.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleCategorical(ColorMap::Categories20);
  cm.value("Categories20", ColorMap::Categories20, docstr.c_str());


  docstr = R"docstr(
    Perceptually uniform sequential color map suitable for viewers with
    color vision deficiency (CVD). Published by
    `Nuñez, Anderton and Renslow <https://doi.org/10.1371/journal.pone.0199239>`__.

    .. image:: ../images/colormap-gradient-cividis.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Cividis);
  cm.value("Cividis", ColorMap::Cividis, docstr.c_str());


  docstr = R"docstr(
    Blue shades from dark to light. This is the *CET-L06* color map by
    `Peter Kovesi <https://colorcet.com/index.html>`__, who released
    under the `CC-BY 4.0 license <https://creativecommons.org/licenses/by/4.0/legalcode>`__.

    .. image:: ../images/colormap-gradient-cold.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Cold);
  cm.value("Cold", ColorMap::Cold, docstr.c_str());


  docstr = R"docstr(
    Perceptually uniform diverging color map for protanopic/deuteranopic
    viewers. This is the *CET-CBD1* color map by
    `Peter Kovesi <https://colorcet.com/index.html>`__, who released
    under the `CC-BY 4.0 license <https://creativecommons.org/licenses/by/4.0/legalcode>`__.

    .. image:: ../images/colormap-gradient-cvd-diverging.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::ColorBlindDiverging);
  cm.value(
        "ColorBlindDiverging", ColorMap::ColorBlindDiverging, docstr.c_str());


  docstr = R"docstr(
    Perceptually uniform sequential color map for protanopic/deuteranopic
    viewers. This is the *CET-CBL1* color map by
    `Peter Kovesi <https://colorcet.com/index.html>`__, who released
    under the `CC-BY 4.0 license <https://creativecommons.org/licenses/by/4.0/legalcode>`__.

    .. image:: ../images/colormap-gradient-cvd-sequential.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::ColorBlindSequential);
  cm.value(
        "ColorBlindSequential", ColorMap::ColorBlindSequential,
        docstr.c_str());


  docstr = R"docstr(
    Perceptually uniform sequential color map with maximal chroma, suitable
    for Protanopic/Deuteranopic viewers. This is the *CET-CBL2* color map by
    `Peter Kovesi <https://colorcet.com/index.html>`__, who released
    under the `CC-BY 4.0 license <https://creativecommons.org/licenses/by/4.0/legalcode>`__.

    .. image:: ../images/colormap-gradient-cvd-sequential-vivid.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::ColorBlindSequentialVivid);
  cm.value(
        "ColorBlindSequentialVivid", ColorMap::ColorBlindSequentialVivid,
        docstr.c_str());


  docstr = R"docstr(
    Cyclic blue-white-yellow-black color map for four orientations/phase
    angles, suitable for Protanopic/Deuteranopic viewers.
    This is the *CET-CBC1* color map by
    `Peter Kovesi <https://colorcet.com/index.html>`__, who released
    under the `CC-BY 4.0 license <https://creativecommons.org/licenses/by/4.0/legalcode>`__.

    .. image:: ../images/colormap-gradient-cvd-orientation.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleOrientation(ColorMap::ColorBlindOrientation);
  cm.value(
        "ColorBlindOrientation", ColorMap::ColorBlindOrientation,
        docstr.c_str());


  docstr = R"docstr(
    Sequential color map from black to light copper. This color map has
    kinks in the lightness curve, which can lead to a perceived banding of
    the data in those value ranges. Based on
    `matplotlib's <https://matplotlib.org>`__ *copper* map.

    .. image:: ../images/colormap-gradient-copper.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Copper);
  cm.value("Copper", ColorMap::Copper, docstr.c_str());


  docstr = R"docstr(
    Placeholder to support using your own color map, see
    :func:`~viren2d.set_custom_colormap`.
    )docstr" + ColorMapCodeExampleCustom(ColorMap::Custom1);
  cm.value("Custom1", ColorMap::Custom1, docstr.c_str());


  docstr = R"docstr(
    Placeholder to support using your own color map, see
    :func:`~viren2d.set_custom_colormap`.
    )docstr" + ColorMapCodeExampleCustom(ColorMap::Custom2);
  cm.value("Custom2", ColorMap::Custom2, docstr.c_str());


  docstr = R"docstr(
    Placeholder to support using your own color map, see
    :func:`~viren2d.set_custom_colormap`.
    )docstr" + ColorMapCodeExampleCustom(ColorMap::Custom3);
  cm.value("Custom3", ColorMap::Custom3, docstr.c_str());


  docstr = R"docstr(
    High contrast color map for depth & disparity images.
    Based on `disparity` for MATLAB
    `by Oliver Woodford <https://github.com/ojwoodford/sc>`__, who released
    it under the `BSD 3-Clause license <https://opensource.org/licenses/BSD-3-Clause>`__.

    .. image:: ../images/colormap-gradient-disparity.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Disparity);
  cm.value("Disparity", ColorMap::Disparity, docstr.c_str());


  docstr = R"docstr(
    A blue-green-brown color map to visualize topography data. Based on
    `matplotlib's <https://matplotlib.org>`__ *gist_earth* map.

    .. image:: ../images/colormap-gradient-earth.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Earth);
  cm.value("Earth", ColorMap::Earth, docstr.c_str());


  docstr = R"docstr(
    Color map for categorical data, best suited for **light backgrounds**,
    *i.e.* light colors are omitted. This color map is adapted from the
    `colorcet Python package <https://github.com/holoviz/colorcet>`__
    and was initially created using
    `Glasbey's method <https://doi.org/10.1002/col.20327>`__.

    .. image:: ../images/colormap-gradient-glasbey-dark.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleCategorical(ColorMap::GlasbeyDark);
  cm.value("GlasbeyDark", ColorMap::GlasbeyDark, docstr.c_str());


  docstr = R"docstr(
    Color map for categorical data, best suited for **dark backgrounds**,
    *i.e.* dark colors are omitted. This color map is adapted from the
    `colorcet Python package <https://github.com/holoviz/colorcet>`__
    and was initially created using
    `Glasbey's method <https://doi.org/10.1002/col.20327>`__.

    .. image:: ../images/colormap-gradient-glasbey-light.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleCategorical(ColorMap::GlasbeyLight);
  cm.value("GlasbeyLight", ColorMap::GlasbeyLight, docstr.c_str());


  docstr = R"docstr(
    Black-blue-green-orange-yellow perceptually uniform sequential color
    map. Similar to MATLAB's `parula`, but with a smoother path and more
    uniform slope upwards in CIELAB space. This is the *CET-L20* color map by
    `Peter Kovesi <https://colorcet.com/index.html>`__, who released
    under the `CC-BY 4.0 license <https://creativecommons.org/licenses/by/4.0/legalcode>`__.

    .. image:: ../images/colormap-gradient-gouldian.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Gouldian);
  cm.value("Gouldian", ColorMap::Gouldian, docstr.c_str());


  docstr = R"docstr(
    Standard grayscale range from black-to-white, *a.k.a.*
    *white-hot*.

    .. image:: ../images/colormap-gradient-gray.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Gray);
  cm.value("Gray", ColorMap::Gray, docstr.c_str());


  docstr = R"docstr(
    Black-purple-red-yellow-white, perceptually uniform sequential color
    map. Similar to `Inferno` but starts from black and ends white.
    Based on `hell` from `Agama <https://github.com/GalacticDynamics-Oxford/Agama>`__.

    .. image:: ../images/colormap-gradient-hell.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Hell);
  cm.value("Hell", ColorMap::Hell, docstr.c_str());


  docstr = R"docstr(
    Black-red-yellow-white perceptually uniform sequential color map.
    This is the *CET-L03* color map by
    `Peter Kovesi <https://colorcet.com/index.html>`__, who released
    under the `CC-BY 4.0 license <https://creativecommons.org/licenses/by/4.0/legalcode>`__.

    .. image:: ../images/colormap-gradient-hot.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Hot);
  cm.value("Hot", ColorMap::Hot, docstr.c_str());


  docstr = R"docstr(
    Cyclic color map obtained by varying the hue. Cycles through
    red-yellow-green-cyan-blue-magenta-red.

    .. image:: ../images/colormap-gradient-hsv.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleOrientation(ColorMap::HSV);
  cm.value("HSV", ColorMap::HSV, docstr.c_str());


  docstr = R"docstr(
    Bluish-to-reddish, perceptually uniform sequential color map with monotonically
    increasing luminance. Proposed by
    `Stéfan van der Walt and Nathaniel Smith <https://bids.github.io/colormap/>`__
    and integrated into `matplotlib >= 1.15 <https://matplotlib.org/>`__.

    .. image:: ../images/colormap-gradient-inferno.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Inferno);
  cm.value("Inferno", ColorMap::Inferno, docstr.c_str());


  docstr = R"docstr(
    The classic rainbow color map, based on
    `MATLAB's <https://www.mathworks.com/help/matlab/ref/jet.html>`__
    *jet* map.

    Note that this color map has several known limitations (*e.g.* reversed
    lightness gradients at yellow and red affect the viewer's perceptual
    ordering). Refer to the excellent article by
    `Peter Kovesi <https://doi.org/10.48550/arXiv.1509.03700>`__ for more
    details about these issues.

    .. image:: ../images/colormap-gradient-jet.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Jet);
  cm.value("Jet", ColorMap::Jet, docstr.c_str());

  docstr = R"docstr(
    A green-blue color map to visualize water depths. Based on
    `matplotlib's <https://matplotlib.org>`__ *ocean* map.

    .. image:: ../images/colormap-gradient-ocean.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Ocean);
  cm.value("Ocean", ColorMap::Ocean, docstr.c_str());


  docstr = R"docstr(
    Cyclic HSV-like color map with reduced colors. Suitable to visualize
    optical flow fields. Based on the color map used by the
    `Middlebury Optical Flow benchmark <https://vision.middlebury.edu/flow/>`__.

    .. image:: ../images/colormap-gradient-optical-flow.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleOrientation(ColorMap::OpticalFlow);
  cm.value("OpticalFlow", ColorMap::OpticalFlow, docstr.c_str());


  docstr = R"docstr(
    Cyclic magenta-yellow-green-blue color map for four orientations/phase
    angles to be visualized. This is the *CET-C2* color map by
    `Peter Kovesi <https://colorcet.com/index.html>`__, who released
    under the `CC-BY 4.0 license <https://creativecommons.org/licenses/by/4.0/legalcode>`__.

    .. image:: ../images/colormap-gradient-orientation-4.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleOrientation(ColorMap::Orientation4);
  cm.value("Orientation4", ColorMap::Orientation4, docstr.c_str());


  docstr = R"docstr(
    Six-color cyclic map with primaries and secondaries matched in
    lightness. This is the *CET-C6* color map by
    `Peter Kovesi <https://colorcet.com/index.html>`__, who released
    under the `CC-BY 4.0 license <https://creativecommons.org/licenses/by/4.0/legalcode>`__.

    .. image:: ../images/colormap-gradient-orientation-6.png
        :width: 266
        :alt: Color map gradient
        :align: center
    )docstr" + ColorMapCodeExampleOrientation(ColorMap::Orientation6);
  cm.value("Orientation6", ColorMap::Orientation6, docstr.c_str());


  docstr = R"docstr(
    The "least worse" rainbow color map, *i.e.* *CET-R2*, by
    `Peter Kovesi <https://colorcet.com/index.html>`__, who released
    under the `CC-BY 4.0 license <https://creativecommons.org/licenses/by/4.0/legalcode>`__.

    .. image:: ../images/colormap-gradient-rainbow.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Rainbow);
  cm.value("Rainbow", ColorMap::Rainbow, docstr.c_str());


  docstr = R"docstr(
    Green-brownish-bluish perceptually uniform sequential color map for
    relief shading. This is the *CET-L11* color map by
    `Peter Kovesi <https://colorcet.com/index.html>`__, who released
    under the `CC-BY 4.0 license <https://creativecommons.org/licenses/by/4.0/legalcode>`__.

    .. image:: ../images/colormap-gradient-relief.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Relief);
  cm.value("Relief", ColorMap::Relief, docstr.c_str());


  docstr = R"docstr(
    Low contrast (isoluminant), blue-green-orange perceptually uniform
    sequential color map for relief shading. This is the *CET-I2* color map by
    `Peter Kovesi <https://colorcet.com/index.html>`__, who released
    under the `CC-BY 4.0 license <https://creativecommons.org/licenses/by/4.0/legalcode>`__.

    .. image:: ../images/colormap-gradient-relief-low-contrast.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::ReliefLowContrast);
  cm.value(
        "ReliefLowContrast", ColorMap::ReliefLowContrast, docstr.c_str());


  docstr = R"docstr(
    Diverging blue-white-red map with vivid colors. Based on
    `matplotlib's <https://matplotlib.org>`__ *seismic* map.

    .. image:: ../images/colormap-gradient-seismic.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Seismic);
  cm.value("Seismic", ColorMap::Seismic, docstr.c_str());


  docstr = R"docstr(
    Color map covering the spectral colors from ultra-violett to infrared.
    Based on `matplotlib's <https://matplotlib.org>`__ *NIPY Spectral* map.

    .. image:: ../images/colormap-gradient-spectral.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Spectral);
  cm.value("Spectral", ColorMap::Spectral, docstr.c_str());


  docstr = R"docstr(
    Diverging redish-yellow-bluish map. Based on
    `matplotlib's <https://matplotlib.org>`__ *Spectral* map.

    .. image:: ../images/colormap-gradient-spectral-diverging.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::SpectralDiverging);
  cm.value("SpectralDiverging", ColorMap::SpectralDiverging, docstr.c_str());



  docstr = R"docstr(
    Sequential pink-to-yellow color map. This color map is not perceptually
    uniform. Based on `matplotlib's <https://matplotlib.org>`__ *spring*
    map.

    .. image:: ../images/colormap-gradient-spring.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Spring);
  cm.value("Spring", ColorMap::Spring, docstr.c_str());


  docstr = R"docstr(
    Sequential green-to-yellow color map. This color map is not perceptually
    uniform. Based on `matplotlib's <https://matplotlib.org>`__ *summer*
    map.

    .. image:: ../images/colormap-gradient-summer.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Summer);
  cm.value("Summer", ColorMap::Summer, docstr.c_str());


  docstr = R"docstr(
    Diverging blue-white-red color map.
    This is the *CET-D01A* color map by
    `Peter Kovesi <https://colorcet.com/index.html>`__, who released
    under the `CC-BY 4.0 license <https://creativecommons.org/licenses/by/4.0/legalcode>`__.

    .. image:: ../images/colormap-gradient-temperature.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Temperature);
  cm.value("Temperature", ColorMap::Temperature, docstr.c_str());


  docstr = R"docstr(
    Diverging blue-black-red color map.
    This is the *CET-D04* color map by
    `Peter Kovesi <https://colorcet.com/index.html>`__, who released
    under the `CC-BY 4.0 license <https://creativecommons.org/licenses/by/4.0/legalcode>`__.

    .. image:: ../images/colormap-gradient-temperature-dark.png
        :width: 266
        :alt: Color map gradient
        :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::TemperatureDark);
  cm.value("TemperatureDark", ColorMap::TemperatureDark, docstr.c_str());


  docstr = R"docstr(
    A blue-green-yellowish-brownish color map to visualize topography
    data. Based on `matplotlib's <https://matplotlib.org>`__ *terrain* map.

    .. image:: ../images/colormap-gradient-terrain.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Terrain);
  cm.value("Terrain", ColorMap::Terrain, docstr.c_str());


  docstr = R"docstr(
    Black-purple-red-yellow-white sequential color map for
    thermographic images, also known as *iron* or *ironbow*.

    .. image:: ../images/colormap-gradient-thermal.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Thermal);
  cm.value("Thermal", ColorMap::Thermal, docstr.c_str());


  docstr = R"docstr(
    A rainbow color map similar to the well-known MATLAB `jet`, but following
    a smoother path through the CIELAB color space. Published by
    `Google (Anton Mikhailov) <https://ai.googleblog.com/2019/08/turbo-improved-rainbow-colormap-for.html>`__
    under the `Apache-2.0 license <https://www.apache.org/licenses/LICENSE-2.0>`__.
    `Turbo` is often used for depth and disparity values.

    .. image:: ../images/colormap-gradient-turbo.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Turbo);
  cm.value("Turbo", ColorMap::Turbo, docstr.c_str());


  docstr = R"docstr(
    Cyclic white-blue-black-red-white map with perceptually uniform lightness
    and color contrast over the whole range. Based on
    `matplotlib's <https://matplotlib.org>`__ *twilight* map.

    .. image:: ../images/colormap-gradient-twilight.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleOrientation(ColorMap::Twilight);
  cm.value("Twilight", ColorMap::Twilight, docstr.c_str());


  docstr = R"docstr(
    Cyclic black-blue-white-red-black map with perceptually uniform lightness
    and color contrast over the whole range. Based on
    `matplotlib's <https://matplotlib.org>`__ *twilight_shifted* map.

    .. image:: ../images/colormap-gradient-twilight-shifted.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleOrientation(ColorMap::TwilightShifted);
  cm.value("TwilightShifted", ColorMap::TwilightShifted, docstr.c_str());


  docstr = R"docstr(
    Perceptually uniform sequential color map. Proposed by
    `Stéfan van der Walt and Nathaniel Smith, based on a design by Eric Firing <https://bids.github.io/colormap/>`__,
    now the default color map of `matplotlib <https://matplotlib.org/>`__.

    .. image:: ../images/colormap-gradient-viridis.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Viridis);
  cm.value("Viridis", ColorMap::Viridis, docstr.c_str());


  docstr = R"docstr(
    Perceptually uniform sequential color map for water depth.
    This is the *CET-L12* color map by
    `Peter Kovesi <https://colorcet.com/index.html>`__, who released
    under the `CC-BY 4.0 license <https://creativecommons.org/licenses/by/4.0/legalcode>`__.

    .. image:: ../images/colormap-gradient-water.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Water);
  cm.value("Water", ColorMap::Water, docstr.c_str());


  docstr = R"docstr(
    Sequential blue-to-light-green color map. This color map is not
    perceptually uniform. Based on
    `matplotlib's <https://matplotlib.org>`__ *winter* map.

    .. image:: ../images/colormap-gradient-winter.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr";
  cm.value("Winter", ColorMap::Winter, docstr.c_str());


  docstr = R"docstr(
    Inverted grayscale range from white-to-black, *a.k.a.*
    *black-hot*.

    .. image:: ../images/colormap-gradient-yarg.png
       :width: 266
       :alt: Color map gradient
       :align: center
    )docstr" + ColorMapCodeExampleScaled(ColorMap::Yarg);
  cm.value("Yarg", ColorMap::Yarg, docstr.c_str());


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
      values. This list **will not** include the customizable
      enumeration values.

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


void SetCustomColorMapHelper(
    const py::object &colormap, const std::vector<Color> &colors) {
  ColorMap cm = ColorMapFromPyObject(colormap);
  SetCustomColorMap(cm, colors);
}


std::vector<Color> GetColorMapColorsHelper(const py::object &colormap) {
  ColorMap cm = ColorMapFromPyObject(colormap);
  return GetColorMapColors(cm);
}


std::vector<Color> ColorizeScalarsHelper(
    const std::vector<double> &values, const py::object &colormap,
    double limit_low, double limit_high, int bins) {
  ColorMap cm = ColorMapFromPyObject(colormap);
  return ColorizeScalars(values, cm, limit_low, limit_high, bins);
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
      Utility class for *scaled colorization* of a data stream.

      This class takes care of computing/storing the input data limits, the
      selected color map, *etc.* This comes in handy whenever we need to apply
      the same colorization over and over again. For example, think of
      displaying the live stream of a time-of-flight sensor.

      **Corresponding C++ API:** ``viren2d::StreamColorizer``.

      Example:
        >>> # Open depth camera stream
        >>> depth_cam = ...
        >>> # Configure stream colorization with fixed data limits
        >>> colorizer = viren2d.StreamColorizer(
        >>>     colormap='turbo', mode='fixed', bins=32,
        >>>     output_channels=3, low=0, high=5000)
        >>> # Colorize the incoming data stream
        >>> while depth_cam.is_available():
        >>>     depth = depth_cam.next()
        >>>     vis = colorizer.colorize(depth)
        >>>     # Or alternatively use the call operator:
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
          low: Lower limit of the input data as :class:`float`. Will only
            be considered if ``mode`` is :attr:`LimitsMode.Fixed`.
          high: Upper limit of the input data as :class:`float`. Will only
            be considered if ``mode`` is :attr:`LimitsMode.Fixed`.
        )docstr",
        py::arg("colormap"),
        py::arg("mode") = "continuous",
        py::arg("bins") = 256,
        py::arg("output_channels") = 3,
        py::arg("low") = std::numeric_limits<double>::infinity(),
        py::arg("high") = std::numeric_limits<double>::infinity())
      .def(
        "__call__",
        [](StreamColorizer &sc, const ImageBuffer &data) {
          return sc(data);
        }, R"docstr(
        Alias of :meth:`~viren2d.StreamColorizer.colorize`.
        )docstr",
        py::arg("data"))
      .def(
        "colorize",
        [](StreamColorizer &sc, const ImageBuffer &data) {
          return sc(data);
        }, R"docstr(
        Applies the configured scaled colorization.

        **Corresponding C++ API:** ``viren2d::StreamColorizer::Colorize``.

        Args:
          data: A single channel :class:`~viren2d.ImageBuffer` or
            :class:`numpy.ndarray` holding the data for colorization.

        Returns:
          The colorization as :class:`~viren2d.ImageBuffer` of type
          :class:`numpy.uint8` with
          :attr:`~viren2d.StreamColorizer.output_channels` channels.
        )docstr",
        py::arg("data"))
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
          selected :attr:`colormap` has less than :attr:`bins` colors.

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


  m.def("get_colormap",
        &GetColorMapColorsHelper, R"docstr(
        Returns the :class:`list` of :class:`~viren2d.Color` for the
        specified color map.

        **Corresponding C++ API:** ``viren2d::GetColorMapColors``.

        Args:
          colormap: The :class:`~viren2d.ColorMap` enumeration value or its
            string representation.

        Example:
          >>> colors = viren2d.get_colormap('ocean')
        )docstr",
        py::arg("colormap"));


  //TODO refactor example code snippet
  m.def("set_custom_colormap",
        &SetCustomColorMapHelper, R"docstr(
        Registers a customized color map.

        Allows library users to register their own color maps for the
        enumeration values :attr:`ColorMap.Custom1`, :attr:`ColorMap.Custom2`,
        and :attr:`ColorMap.Custom3`.

        **Corresponding C++ API:** ``viren2d::SetCustomColorMap``.

        Args:
          id: The :class:`~viren2d.ColorMap` enumeration value under which to
            register the color map. Also accepts the corresponding string
            representation.
          colors: The color map as :class:`list` of :class:`~viren2d.Color`.
            Note that a :class:`~viren2d.Color` is defined by
            :math:`r,g,b \in [0,1]`.

        Example:
          >>> # Exemplary categorical data for visualization
          >>> import numpy as np
          >>> labels = np.array(
          >>>     [[0, 1, 2], [-3, -4, -6], [20000, 20001, 20003]],
          >>>     dtype=np.int32)
          >>> # Register a custom color map. This is a usage example and by
          >>> # no means a useful color map!
          >>> viren2d.set_custom_colormap(
          >>>     'custom1', ['#800000', (0.94, 0.13, 0.15), 'rose-red'])
          >>> # Apply the custom map for label colorization:
          >>> vis = viren2d.colorize_labels(labels=labels, colormap='custom1')
        )docstr",
        py::arg("id"),
        py::arg("colors"));


  std::string docstr = R"docstr(
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

)docstr" + DocstringCodeExample("colorization-scaled")
+ "\n\n|image-colorized-peaks|";

  m.def("colorize_scaled",
        &ColorizationScaledHelper, docstr.c_str(),
        py::arg("data"),
        py::arg("colormap") = ColorMap::Gouldian,
        py::arg("low") = std::numeric_limits<double>::infinity(),
        py::arg("high") = std::numeric_limits<double>::infinity(),
        py::arg("output_channels") = 3,
        py::arg("bins") = 256);


  docstr = R"docstr(
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

)docstr" + DocstringCodeExample("colorization-categorical")
+ "\n\n|image-label-colorization|";

  m.def("colorize_labels",
        &ColorizationLabelsHelper, docstr.c_str(),
        py::arg("labels"),
        py::arg("colormap") = ColorMap::GlasbeyDark,
        py::arg("output_channels") = 3);


  docstr = R"docstr(
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

)docstr" + DocstringCodeExample("relief-shading")
      + "\n\n|image-relief-shading|";
  m.def("relief_shading",
        &ReliefShading,
        docstr.c_str(),
        py::arg("relief"),
        py::arg("colorized"));


  //TODO add code example
  m.def("peaks",
        &Peaks, R"docstr(
        Computes the `peaks` example data.

        Returns exemplary data for visualization from translated and scaled
        Gaussian distributions, known from
        `MATLAB <https://www.mathworks.com/help/matlab/ref/peaks.html>`__:
        :math:`\operatorname{peaks}(x,y) = 3 \left(1 - x\right)^2 e^{-x^2 - (y+1)^2} - 10 \left( \frac{x}{5} - x^3 - y^5 \right) e^{-x^2 - y^2} - \frac{1}{3} e^{-(x+1)^2 - y^2}`.

        **Corresponding C++ API:** ``viren2d::Peaks``.

        Returns:
          A ``width`` by ``height`` single-channel :class:`~viren2d.ImageBuffer`
          of type :class:`numpy.float64`.
        )docstr",
        py::arg("height") = 600,
        py::arg("width") = 600);


  docstr = R"docstr(
Returns corresponding color map colors for a list of scalar values.

Performs scaled colorization similar to :func:`~viren2d.colorize_scaled`,
but operates on a list of scalars (instead of a :class:`numpy.ndarray`)
and returns a list of :class:`~viren2d.Color` objects.

**Corresponding C++ API:** ``viren2d::ColorizeScalars``.

Args:
  values: The input list of scalar values.
  colormap: The :class:`~viren2d.ColorMap` to be used for
    colorization. In addition to the enumeration value, its
    string representation can be used for convenience.
  low: Lower limit of the input data as :class:`float`. If :math:`\infty`
    or *NaN*, it will be computed from the input data.
  high: Upper limit of the input data as :class:`float`. If :math:`\infty`
    or *NaN*, it will be computed from the input data.
  bins: Number of discretization bins as :class:`int`.
    Must be :math:`\geq 2`. This parameter will be ignored if the
    selected color map has less than ``bins`` colors.

Returns:
  A :class:`list` of :class:`~viren2d.Color` objects.

)docstr" + DocstringCodeExample("colorization-scalars");
  m.def("colorize_scalars",
        &ColorizeScalarsHelper,
        docstr.c_str(),
        py::arg("values"),
        py::arg("colormap") = ColorMap::Gouldian,
        py::arg("low") = std::numeric_limits<double>::infinity(),
        py::arg("high") = std::numeric_limits<double>::infinity(),
        py::arg("bins") = 256);

}
} // namespace bindings
} // namespace viren2d

