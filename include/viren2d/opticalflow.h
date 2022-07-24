#ifndef __VIREN2D_OPTICALFLOW_H__
#define __VIREN2D_OPTICALFLOW_H__

#include <string>

#include <viren2d/imagebuffer.h>
#include <viren2d/colormaps.h>
#include <viren2d/styles.h>

namespace viren2d {

/// Loads a `.flo` optical flow file into a two-band image buffer.
ImageBuffer LoadOpticalFlow(const std::string &filename);


/// Saves a two-band optical flow image as `.flo` file.
void SaveOpticalFlow(const std::string &filename, const ImageBuffer &flow);


/// Colorizes a two-band optical flow field.
/// Given a cyclic color map, this returns the false color representation,
/// where the flow orientation defines the color map bin and the magnitude
/// defines the corresponding color's saturation. Output will always be
/// an ImageBuffer of type uint8.
///
/// This assumes that flow is normalized such that the maximum magnitude
/// is 1. Larger motion will be indicated by a desaturated color. To avoid
/// this, you can adjust the `motion_normalizer` parameter which is used
/// to divide the flow magnitude.
///
/// The default color map is the cyclic six-color map CET-C6 proposed by
/// `Peter Kovesi <https://doi.org/10.48550/arXiv.1509.03700>`__.
///
/// Args:
///   flow: The optical flow field as 2-channel ImageBuffer of type float
///     or double.
///   colormap: Ideally, a cyclic color map.
///   motion_normalizer: Used to divide the flow magnitude. Set to the
///     maximum motion magnitude to avoid desaturation in regions where the
///     flow magnitude would be > 1.
///   output_channels: Number of output channels. Must be either 3 or 4.
///     If a fourth channel is requested, it is considered an alpha
///     channel and set to 255.
ImageBuffer ColorizeOpticalFlow(const ImageBuffer &flow,
    ColorMap colormap = ColorMap::Orientation6,
    double motion_normalizer = 1.0,
    int output_channels = 3);


// TODO returns uint8
ImageBuffer OpticalFlowLegend(
    int size,
    ColorMap colormap = ColorMap::Orientation6,
    LineStyle line_style = LineStyle::Invalid,
    bool draw_circle = false,
    bool clip_circle = false,
    int output_channels = 3);

} // namespace viren2d

#endif //__VIREN2D_OPTICALFLOW_H__
