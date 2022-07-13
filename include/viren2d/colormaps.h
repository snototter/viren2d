#ifndef __VIREN2D_COLORMAPS_H__
#define __VIREN2D_COLORMAPS_H__

#include <string>
#include <ostream>
#include <vector>
#include <array>
#include <limits>

#include <viren2d/imagebuffer.h>


namespace viren2d {

/// Available colormaps.
enum class ColorMap : unsigned char
{
  /// Black-red-yellow-white, perceptually uniform sequential color map
  /// inspired by black-body radiation. This color map definition has
  /// been taken from
  /// `Kenneth Moreland's website <https://www.kennethmoreland.com/color-advice/>`__.
  BlackBody,

  /// Color map with 10 distinct colors, suitable for categorical data. Based
  /// on `matplotlib's <https://matplotlib.org>`__ *tab10* map.
  Categories10,

  /// Color map with 20 distinct colors, suitable for categorical data. Based
  /// on `matplotlib's <https://matplotlib.org>`__ *tab20* map.
  Categories20,

  /// Blue shades from dark to light. This is the CET-L06 color map by
  /// `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
  /// under the CC-BY 4.0 license.
  Cold,

  /// Perceptually uniform sequential color map for Protanopic/Deuteranopic
  /// viewers. This is the CET-CBL1 color map by
  /// `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
  /// under the CC-BY 4.0 license.
  ColorBlind,


  //TODO
  Copper,


  /// High contrast color map for depth & disparity images.
  /// Based on `disparity` for MATLAB
  /// `by Oliver Woodford <https://github.com/ojwoodford/sc>`__,
  /// who released it under the BSD 3-Clause license.
  Disparity,

  /// A blue-green-brown color map to visualize topography data. Based on
  /// `matplotlib's <https://matplotlib.org>`__ *gist_earth* map.
  Earth,

  /// Color map for categorical data, best suited for **light backgrounds**,
  /// i.e. light colors are omitted. This color map is adapted from
  /// `colorcet <https://github.com/holoviz/colorcet>`__ and was created using
  /// `Glasbey's method <https://strathprints.strath.ac.uk/30312/1/colorpaper_2006.pdf>`.
  GlasbeyDark,

  /// Color map for categorical data, best suited for **dark backgrounds**,
  /// i.e. dark colors are omitted. This color map is adapted from
  /// `colorcet <https://github.com/holoviz/colorcet>`__ and was created using
  /// `Glasbey's method <https://strathprints.strath.ac.uk/30312/1/colorpaper_2006.pdf>`.
  GlasbeyLight,

  /// Black-blue-green-orange-yellow perceptually uniform sequential color
  /// map. Similar to MATLAB's `parula`, but with a smoother path and more
  /// uniform slope upwards in CIELAB space. This is the CET-L20 color map by
  /// `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
  /// under the CC-BY 4.0 license.
  Gouldian,

  /// Standard grayscale from black-to-white.
  Gray,

  /// Black-purple-red-yellow-white, perceptually uniform sequential color
  /// map. Similar to `Inferno` but starts from black and ends white.
  /// Based on `hell` from `Agama <https://github.com/GalacticDynamics-Oxford/Agama>`__.
  Hell,

  /// Black-red-yellow-white perceptually uniform sequential color map.
  /// This is the CET-L03 color map by
  /// `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
  /// under the CC-BY 4.0 license.
  Hot,

  /// Cyclic color map by varying the hue: Red-yellow-green-cyan-blue-magenta-red.
  HSV,

  /// Bluish-to-reddish, perceptually uniform sequential color map. Proposed by
  /// `Stéfan van der Walt and Nathaniel Smith <https://bids.github.io/colormap/>`__
  /// and integrated into matplotlib >= 1.15.
  Inferno,

  /// A green-blue color map to visualize water depths. Based on
  /// `matplotlib's <https://matplotlib.org>`__ *ocean* map.
  Ocean,

  /// Cyclic HSV-like color map with reduced colors. Suitable to visualize
  /// optical flow fields. Based on the color map used by the
  /// `Middlebury Optical Flow benchmark <https://vision.middlebury.edu/flow/>`__.
  OpticalFlow,

  /// Cyclic magenta-yellow-green-blue color map for four orientations/phase
  /// angles to be visualized. This is the CET-C2 color map by
  /// `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
  /// under the CC-BY 4.0 license.
  Orientation,

  /// Cyclic blue-white-yellow-black color map for four orientations/phase
  /// angles, suitable for Protanopic/Deuteranopic viewers.
  /// This is the CET-CBC1 color map by
  /// `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
  /// under the CC-BY 4.0 license.
  OrientationColorBlind,

  /// The "least worse" rainbow color map, i.e. CET-R2, by
  /// `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
  /// under the CC-BY 4.0 license.
  Rainbow,

  /// Green-brownish-bluish perceptually uniform sequential color map for
  /// relief shading. This is the CET-L11 color map by
  /// `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
  /// under the CC-BY 4.0 license.
  Relief,

  /// Low contrast (isoluminant), blue-green-orange perceptually uniform
  /// sequential color map for relief shading. This is the CET-I2 color map by
  /// `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
  /// under the CC-BY 4.0 license.
  ReliefLowContrast,

  /// Diverging blue-white-red color map.
  /// This is the CET-D01A color map by
  /// `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
  /// under the CC-BY 4.0 license.
  Temperature,

  /// Diverging blue-black-red color map.
  /// This is the CET-D04 color map by
  /// `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
  /// under the CC-BY 4.0 license.
  TemperatureDark,

  /// A blue-green-yellowish-brownish color map to visualize topography
  /// data. Based on `matplotlib's <https://matplotlib.org>`__ *terrain* map.
  Terrain,

  /// Black-purple-red-yellow-white sequential color map for
  /// thermographic images, also known as `iron` or `ironbow`.
  Thermal,

  /// A rainbow color map similar to the well-known MATLAB `jet`, but following
  /// a smoother path through the CIELAB color space. Published by
  /// `Google (Anton Mikhailov) <https://ai.googleblog.com/2019/08/turbo-improved-rainbow-colormap-for.html>`__
  /// under the Apache-2.0 license.
  Turbo,

  /// Perceptually uniform sequential color map. Proposed by
  /// `Stéfan van der Walt and Nathaniel Smith <https://bids.github.io/colormap/>`__,
  /// now the default color map of matplotlib.
  Viridis,

  /// Perceptually uniform sequential color map for water depth.
  /// This is the CET-L12 color map by
  /// `Peter Kovesi <https://colorcet.com/index.html>`__, which was released
  /// under the CC-BY 4.0 license.
  Water,

  /// Inverted grayscale from white-to-black.
  Yarg

  //TODO(dev): If you add an enum value *after* Viridis, adjust the iterator TODO in ListColorMaps - later on in the test suite, too!
};

//TODO change color::by_id/category to use glasbey
//TODO consider adding mpl ocean & terrain

/// Returns the string representation.
std::string ColorMapToString(ColorMap cm);


/// Returns the ColorMap enum from its string representation.
ColorMap ColorMapFromString(const std::string &cm);


/// Output stream operator to print a ColorMap enum.
std::ostream &operator<<(std::ostream &os, ColorMap cm);


/// Returns a list of all available color maps.
std::vector<ColorMap> ListColorMaps();


/// Utility class to simplify colorization of a data stream.
/// This class takes care of computing/storing the limits,
/// color map, etc. so your client code doesn't need to.
class Colorizer {
public:

  enum class LimitsMode {
    FromDataContinuously = 0,
    FromDataOnce,
    Fixed
  };

  Colorizer(
      ColorMap cmap,
      LimitsMode mode = LimitsMode::FromDataContinuously,
      int num_bins = 256,
      int channels_out = 3,
      double low = std::numeric_limits<double>::infinity(),
      double high = std::numeric_limits<double>::infinity());

  void SetLimitLow(double low);
  double LimitLow() const { return limit_low; }

  void SetLimitHigh(double high);
  double LimitHigh() const { return limit_high; }

  void SetColorMap(ColorMap cmap);
  ColorMap GetColorMap() const { return colormap; }

  void SetBins(int num_bins);
  int Bins() const { return bins; }

  void SetOutputChannels(int channels_out);
  int OutputChannels() const { return output_channels; }

  ImageBuffer operator()(const ImageBuffer &data);

private:
  ColorMap colormap;
  LimitsMode limits_mode;
  int bins;
  int output_channels;
  double limit_low;
  double limit_high;

  void ValidateConfiguration() const;
};


/// Returns the string representation.
std::string LimitsModeToString(Colorizer::LimitsMode lm);


/// Returns the LimitsMode enum from its string representation.
Colorizer::LimitsMode LimitsModeFromString(const std::string &lm);


/// Colorizes 2D data array using a colormap.
///
/// Args:
///   data: A single channel ImageBuffer holding the data for colorization.
///   colormap: The ColorMap to be used for colorization.
///   low: Lower limit of the input values. If either ``low`` or ``high``
///     are ``inf`` or ``nan``, **both limits** will be computed from the
///     input ``data``.
///   high: Upper limit of the input values.
///   output_channels: Number of output channels. Must be either 3 or 4. The
///     optional 4th channel will be considered an alpha channel and set to 255.
///   bins: Number of discretization bins. Must be `>=2`. This parameter will
///     be ignored if the selected color map has less than ``bins`` colors.
///
/// Returns:
///   The colorization result as UInt8 ImageBuffer.
ImageBuffer Colorize(
    const ImageBuffer &data, ColorMap colormap,
    double limit_low, double limit_high,
    int output_channels = 3, int bins = 256);


// data: single-channel --> will be converted to float/double
// if int/uint will be scaled by 255.0!
// output = colorization * data
ImageBuffer ReliefShading(
    const ImageBuffer &relief, const ImageBuffer &colorized);


/// Computes exemplary data from translated and scaled Gaussian
/// distributions, known from MATLAB's `peaks`. For details on
/// the formal definition, refer to the
/// MATLAB documentation: https://www.mathworks.com/help/matlab/ref/peaks.html
///
/// Returns: A ``width`` by ``height`` single-channel image of type `double`
ImageBuffer Peaks(int height, int width);


} // namespace viren2d

#endif // __VIREN2D_COLORMAPS_H__
