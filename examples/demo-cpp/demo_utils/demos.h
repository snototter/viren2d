#include <string>

#include <viren2d/imagebuffer.h>

namespace viren2d {
namespace demos {
/// Prints each demo's title in a consistent style.
void PrintDemoHeader(const std::string &title);

/// Saves the canvas to disk & optionally displays the image (iff OpenCV is
/// available on your system).
void ProcessDemoOutput(ImageBuffer canvas, const std::string &filename);


void DemoArrows();


} // namespace demos
} // namespace viren2d
