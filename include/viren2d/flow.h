#ifndef __VIREN2D_OPTICALFLOW_H__
#define __VIREN2D_OPTICALFLOW_H__

#include <string>

#include <viren2d/imagebuffer.h>

namespace viren2d {

/// Loads a `.flo` optical flow file into a two-band image.
ImageBuffer LoadOpticalFlow(const std::string &filename);


/// Saves a two-band optical flow image as `.flo` format.
void SaveOpticalFlow(const std::string &filename, const ImageBuffer &flow);


//TODO colorize flow

} // namespace viren2d

#endif //__VIREN2D_OPTICALFLOW_H__
