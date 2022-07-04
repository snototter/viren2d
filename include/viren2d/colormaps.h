#ifndef __VIREN2D_COLORMAPS_H__
#define __VIREN2D_COLORMAPS_H__

#include <string>
#include <ostream>
#include <vector>
#include <limits>

#include <viren2d/imagebuffer.h>


namespace viren2d {

/// Available colormaps.
enum class ColorMap : unsigned char
{
  /// Red-yellow color map, similar to MATLAB's autumn.
  Autumn = 0,

  /// Black-blue-white color map, similar to MATLAB's bone.
  Bone,

  /// Black-blue-cyan-white color map.
  Cold,

  /// High contrast color map with subtle gradient discontinuities, suitable
  /// for depth/disparity images.
  Disparity,

  /// Black-green-white color map.
  /// Has linear grayscale changes when printed in black & white.
  Earth,

  /// Convert input to grayscale
  Grayscale,

  /// Black-red-yellow-white color map, similar to MATLAB's hot.
  Hot,

  /// Red-yellow-green-cyan-blue-magenta-red color map.
  HSV,

  /// Perceptually uniform.
  Inferno,

  /// Rainbow color map. MATLAB's default up until R2015, has high contrast
  /// but rainbow maps should be avoided. See for example:
  /// http://jakevdp.github.io/blog/2014/10/16/how-bad-is-your-colormap/
  Jet,

  /// Perceptually uniform.
  Magma,

  /// Black-pastel-white color map.
  /// Has linear grayscale changes when printed in black & white.
  Pastel,

  /// Perceptually uniform.
  Plasma,

  /// Black-brown-white color map, perceptually uniform.
  Sepia,

  /// Blue-pale-dark red color map, for visualizing data related to
  /// temperature. Has good contrast for colorblind viewers.
  Temperature,

  /// Black-purple-red-yellow-white color map.
  Thermal,

  /// An improved rainbow color map, similar to (but smoother than) Jet.
  /// https://ai.googleblog.com/2019/08/turbo-improved-rainbow-colormap-for.html
  Turbo,

  /// Perceptually uniform. Default color map of matplotlib.
  Viridis

  //TODO(dev): If you add an enum value *after* Viridis, adjust the iterator TODO in ListColormaps - later on in the test suite, too!
};


/// Returns the string representation.
std::string ColorMapToString(ColorMap cm);


/// Returns the ColorMap enum from its string representation.
ColorMap ColorMapFromString(const std::string &cm);


/// Output stream operator to print a ColorMap enum.
std::ostream &operator<<(std::ostream &os, ColorMap cm);

std::vector<ColorMap> ListColorMaps();


class Colorizer {
public:
  enum class LimitsMode {
    FromDataContinuously = 0,
    FromDataOnce,
    Fixed
  };

  Colorizer(
      ColorMap cmap,  // TODO gouldian
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
  ColorMap GetColorMap() const { return color_map; }

  void SetBins(int num_bins);
  int Bins() const { return bins; }

  void SetOutputChannels(int channels_out);
  int OutputChannels() const { return output_channels; }

  ImageBuffer operator()(const ImageBuffer &data);

private:
  ColorMap color_map;
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


// TODO doc num output channels either 3 or 4
ImageBuffer Colorize(
    const ImageBuffer &data, ColorMap color_map,
    double limit_low, double limit_high,
    int output_channels = 3, int bins = 256);


/// Computes exemplary data from translated and scaled Gaussian
/// distributions, known from MATLAB's `peaks`. For details on
/// the formal definition, refer to the
/// MATLAB documentation: https://www.mathworks.com/help/matlab/ref/peaks.html
///
/// Returns: A ``width`` by ``height`` single-channel image of type `double`
ImageBuffer Peaks(int height, int width);


} // namespace viren2d

#endif // __VIREN2D_COLORMAPS_H__
