#ifndef __VIVI_COLORS_H__
#define __VIVI_COLORS_H__

#include <memory>
#include <algorithm>
#include <string>
#include <tuple>
#include <ostream>

//TODO colornames
//TODO colormaps
//TODO pseudocoloring!

//TODO python bindings for colornames

namespace vivi
{

/** @brief Clips the given value to the range [low, high]. */
template<typename _Tp>
_Tp saturation_cast(_Tp val, _Tp low, _Tp high)
{
  return std::min(std::max(val, low), high);
}


//-------------------------------------------------  Color

/**
 * @brief Represents a rgba color
 *
 * Convention:
 * * RGB (upper case letters) are in range [0, 255]
 * * rgb (lower case letters) are in range [0, 1]
 *
 * For painting with Cairo, we need color values within [0, 1].
 * For convenience, you can use the derived @see ColorRGB
 *
 * TODO nice-to-have: add conversions from other color spaces (such as HSV, CMYK)
 *      --> not now, we're almost exclusively working with RGB/rgb
 */
struct Color
{
  double red;   /**< Red component within [0,1]. */
  double green; /**< Green component within [0,1]. */
  double blue;  /**< Blue component within [0,1]. */
  double alpha; /**< Alpha (opacity) component within [0,1]. */

  Color() : red(-1.0), green(-1.0), blue(-1.0), alpha(1.0)
  {}

  Color(double r, double g, double b, double a)
    : red(saturation_cast<double>(r, 0.0, 1.0)),
      green(saturation_cast<double>(g, 0.0, 1.0)),
      blue(saturation_cast<double>(b, 0.0, 1.0)),
      alpha(saturation_cast<double>(a, 0.0, 1.0))
  {}

  /**
   * @brief Returns the inverse color, i.e. (1.0-r, 1.0-g, 1.0-b). Alpha value stays the same.
   *
   * Caveat: TODO 50% gray inverse = 50% gray in rgb color space --> use opposite color
   */
  Color Inverse() const;

  std::string ToString() const;

  /** @brief Returns the corresponding (R, G, B, a) tuple, where R, G, B in [0, 255] and alpha in [0, 1]. */
  std::tuple<unsigned char, unsigned char, unsigned char, double>
  ToRGBA() const;


  /** @brief Returns the web color code, e.g. "#dcdce4" (alpha value is ignored). */
  std::string ToHexString() const;

  friend std::ostream &operator<<(std::ostream &os, const Color &col)
  {
    os << col.ToString();
    return os;
  }
};


//-------------------------------------------------  Comparison operators

bool operator==(const Color& lhs, const Color& rhs);
bool operator!=(const Color& lhs, const Color& rhs);


//-------------------------------------------------  Convenience initialization for Color

/** Convenience wrapper to initialize @see Color from rgb values in range [0,1]. */
Color rgba(double r, double g, double b, double alpha=1.0);

/** Convenience wrapper to initialize @see Color from RGB values in range [0,255]. */
Color RGBA(double R, double G, double B, double alpha=1.0);

//TODO group by r,g,b levels?
namespace colors
{
  Color Black(double alpha=1.0);
  Color White(double alpha=1.0);

  Color Crimson(double alpha=1.0);
  Color Maroon(double alpha=1.0);
  Color Purple(double alpha=1.0);
  Color Cyan(double alpha=1.0);
  Color Magenta(double alpha=1.0);
  Color Turquoise(double alpha=1.0);
  Color Orange(double alpha=1.0);
  Color Orchid(double alpha=1.0);
  Color Silver(double alpha=1.0);
  Color Gold(double alpha=1.0);
  Color ForestGreen(double alpha=1.0);
  Color TealGreen(double alpha=1.0);
  Color LimeGreen(double alpha=1.0);
  Color NavyBlue(double alpha=1.0);
  Color Indigo(double alpha=1.0);
  Color Copper(double alpha=1.0);
  Color Freesia(double alpha=1.0);
  Color MidnightBlue(double alpha=1.0);
  Color Salmon(double alpha=1.0);
  Color RoseRed(double alpha=1.0);
  Color Olive(double alpha=1.0);
  Color LightBlue(double alpha=1.0);
  Color Lavender(double alpha=1.0);
  Color Ivory (double alpha=1.0);
} // namespace colors

} // namespace vivi

#endif // __VIVI_COLORS_H__

