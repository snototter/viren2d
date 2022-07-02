#ifndef __VIREN2D_COLORMAPS_H__
#define __VIREN2D_COLORMAPS_H__

#include <string>
#include <ostream>

#include <viren2d/imagebuffer.h>


namespace viren2d {

/// Available color maps.
enum class ColorMap : unsigned char
{
  /// Red-yellow colormap, similar to MATLAB's autumn.
  Autumn = 0,

  /// Black-blue-white colormap, similar to MATLAB's bone.
  Bone,

  /// Black-blue-cyan-white colormap.
  Cold,

  /// High contrast colormap with subtle gradient discontinuities, suitable
  /// for depth/disparity images.
  Disparity,

  /// Black-green-white colormap.
  /// Has linear grayscale changes when printed in black & white.
  Earth,

  /// Convert input to grayscale
  Grayscale,

  /// Black-red-yellow-white colormap, similar to MATLAB's hot.
  Hot,

  /// Red-yellow-green-cyan-blue-magenta-red colormap.
  HSV,

  /// Perceptually uniform.
  Inferno,

  /// Rainbow color map. MATLAB's default up until R2015, has high contrast
  /// but rainbow maps should be avoided. See for example:
  /// http://jakevdp.github.io/blog/2014/10/16/how-bad-is-your-colormap/
  Jet,

  /// Perceptually uniform.
  Magma,

  /// Black-pastel-white colormap.
  /// Has linear grayscale changes when printed in black & white.
  Pastel,

  /// Perceptually uniform.
  Plasma,

  /// Black-brown-white colormap, perceptually uniform.
  Sepia,

  /// Blue-pale-dark red colormap, for visualizing data related to
  /// temperature. Has good contrast for colorblind viewers.
  Temperature,

  /// Black-purple-red-yellow-white colormap.
  Thermal,

  /// An improved rainbow colormap, similar to (but smoother than) Jet.
  /// https://ai.googleblog.com/2019/08/turbo-improved-rainbow-colormap-for.html
  Turbo,

  /// Perceptually uniform. Default colormap of matplotlib.
  Viridis
};


/// Returns the string representation.
std::string ColorMapToString(ColorMap cm);


/// Returns the ColorMap enum from its string representation.
ColorMap ColorMapFromString(const std::string &cm);


/// Output stream operator to print a ColorMap enum.
std::ostream &operator<<(std::ostream &os, ColorMap cm);


// TODO doc num output channels either 3 or 4
ImageBuffer Colorize(const ImageBuffer &data, ColorMap colormap,
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
