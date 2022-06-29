#include <viren2d/viren2d.h>

auto painter = viren2d::CreatePainter();

// Either create an empty canvas (H=600, W=800):
painter->SetCanvas(600, 800, viren2d::Color(viren2d::NamedColor::NavyBlue));

// Initialize with an existing viren2d::ImageBuffer:
painter->SetCanvas(image_buffer);

// Or initialize from an image file:
painter->SetCanvas("path/to/image.jpg");
