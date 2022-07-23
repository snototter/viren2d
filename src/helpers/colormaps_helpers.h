#ifndef __VIREN2D_COLORMAPS_HELPERS_H__
#define __VIREN2D_COLORMAPS_HELPERS_H__

#include <utility>
#include <stdexcept>
#include <sstream>
#include <cstdlib>

#include <viren2d/colormaps.h>


namespace viren2d {
namespace helpers {

/// Color maps are predominantly used to false color inputs, i.e. they will
/// be used to create uint8 images. Thus, we define the color maps as RGB
/// colors with R,G,B in [0, 255] to avoid the unnecessary double --> uint8
/// conversion.
struct RGBColor {
  unsigned char red;
  unsigned char green;
  unsigned char blue;

  constexpr RGBColor(unsigned char R, unsigned char G, unsigned char B)
    : red(R), green(G), blue(B)
  {}

  inline unsigned char operator[](int i) const {
    if (i == 0) {
      return red;
    } else if (i == 1) {
      return green;
    } else if (i == 2) {
      return blue;
    } else {
      std::ostringstream s;
      s << "Index (" << i << ") is out of range for `RGBColor`.";
      throw std::range_error(s.str());
    }
  }
};


/// Returns the list of colors (along with number of colors) for the
/// specified color map.
std::pair<const RGBColor *, std::size_t> GetColorMap(ColorMap colormap);


/// Returns the color for the given category/object id.
inline RGBColor GetCategoryColor(int category_id, ColorMap colormap) {
  std::pair<const RGBColor*, std::size_t> map = GetColorMap(colormap);
  return map.first[static_cast<std::size_t>(category_id) % map.second];
}


/// Registers the given colors under a `ColorMap::Custom#` enumeration value.
void SetUserDefinedColorMap(
    ColorMap colormap, const std::vector<RGBColor> &colors);

} // namespace helpers
} // namespace viren2d

#endif // __VIREN2D_COLORMAPS_HELPERS_H__
