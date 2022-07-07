#ifndef __VIREN2D_COLORMAPS_HELPERS_H__
#define __VIREN2D_COLORMAPS_HELPERS_H__

#include <viren2d/colormaps.h>

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

const RGBColor *GetColorMap(ColorMap colormap);

} // namespace helpers
} // namespace viren2d

#endif // __VIREN2D_COLORMAPS_HELPERS_H__
