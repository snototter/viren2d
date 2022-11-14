#include <stdexcept>
#include <sstream>
#include <cmath>

// public viren2d headers
#include <viren2d/collage.h>

// private viren2d headers
#include <helpers/logging.h>


namespace viren2d {

ImageBuffer Collage(
    const std::vector<std::vector<ImageBuffer>> &images,
    const Vec2i &image_size,
    HorizontalAlignment halign,
    VerticalAlignment valign,
    const Color &bg_color,
    const Vec2i &spacing,
    const Vec2i &margin) {
  SPDLOG_ERROR("Collage: {} rows TODO", images.size());
  for (const auto &row : images) {
    for (const auto &img : row) {
      SPDLOG_CRITICAL("Input image: {:s}", img.ToString());
    }
  }

  throw std::runtime_error("TODO");

  return ImageBuffer();
}

} // namespace viren2d
