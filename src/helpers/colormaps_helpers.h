#ifndef __VIREN2D_COLORMAPS_HELPERS_H__
#define __VIREN2D_COLORMAPS_HELPERS_H__

#include <utility>

#include <viren2d/colormaps.h>

//TODO documentation


namespace viren2d {
namespace helpers {
struct RGBColor {
  unsigned char red;
  unsigned char green;
  unsigned char blue;

  constexpr RGBColor(unsigned char R, unsigned char G, unsigned char B)
    : red(R), green(G), blue(B)
  {}
};

std::pair<const RGBColor *, std::size_t> GetColorMap(ColorMap colormap);

inline RGBColor GetCategoryColor(int category_id, ColorMap colormap) {
  std::pair<const RGBColor*, std::size_t> map = GetColorMap(colormap);
  return map.first[static_cast<std::size_t>(category_id) % map.second];
}

} // namespace helpers
} // namespace viren2d

#endif // __VIREN2D_COLORMAPS_HELPERS_H__
