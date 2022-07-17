#include <string>

#include <viren2d/imagebuffer.h>

namespace viren2d {
namespace demos {
/// Prints each demo's title in a consistent style.
void PrintDemoHeader(const std::string &title);

/// Saves the canvas to disk & optionally displays the image (iff OpenCV is
/// available on your system).
void ProcessDemoOutput(const ImageBuffer &canvas, const std::string &filename);


void DemoArrows();

void DemoBoundingBoxes2D();

void DemoCircles();

void DemoColorMaps();

void DemoImageBufferConversionOpenCV();

void DemoLines();

void DemoMarkers();

void DemoOpticalFlow();

void DemoPolygons();

void DemoRects();

void DemoText();

void DemoTrajectories();

} // namespace demos
} // namespace viren2d
