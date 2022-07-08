#ifndef __VIREN2D_OPTICALFLOW_H__
#define __VIREN2D_OPTICALFLOW_H__

#include <string>

#include <viren2d/imagebuffer.h>
#include <viren2d/colormaps.h>
#include <viren2d/styles.h>

namespace viren2d {

/// Loads a `.flo` optical flow file into a two-band image.
ImageBuffer LoadOpticalFlow(const std::string &filename);


/// Saves a two-band optical flow image as `.flo` format.
void SaveOpticalFlow(const std::string &filename, const ImageBuffer &flow);


//TODO colorize flow, output 3 or 4 channels, flow as either float or double
// orientation defines the bin, magnitude the saturation
// use with hsv (standard flow visualization, or the perceptually uniform Orientation/colorblind maps)
// returns uint8
// motion_normalizer used to divide the motion magnitude - set to max motion to avoid dimming
ImageBuffer ColorizeOpticalFlow(const ImageBuffer &flow,
    ColorMap colormap = ColorMap::Orientation,
    double motion_normalizer = 1.0,
    int output_channels = 3);


// returns uint8
ImageBuffer OpticalFlowLegend(int size, ColorMap colormap = ColorMap::Orientation,
    LineStyle line_style = LineStyle::Invalid, bool draw_circle = false,
    bool clip_circle = false, int output_channels = 3);

} // namespace viren2d

#endif //__VIREN2D_OPTICALFLOW_H__
