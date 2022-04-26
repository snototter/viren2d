#ifndef __VIREN2D_COLORS_H__
#define __VIREN2D_COLORS_H__

#include <memory>
#include <algorithm>
#include <string>
#include <tuple>
#include <ostream>

//TODO colormaps
//TODO pseudocoloring!


namespace viren2d
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

  Color(double r, double g, double b, double alpha_)
    : red(saturation_cast<double>(r, 0.0, 1.0)),
      green(saturation_cast<double>(g, 0.0, 1.0)),
      blue(saturation_cast<double>(b, 0.0, 1.0)),
      alpha(saturation_cast<double>(alpha_, 0.0, 1.0))
  {}

  Color(const Color &other, double alpha_)
    : red(other.red), green(other.green), blue(other.blue),
      alpha(saturation_cast<double>(alpha_, 0.0, 1.0))
  {}

  /**
   * @brief Returns the inverse color, i.e. (1.0-r, 1.0-g, 1.0-b). Alpha value stays the same.
   *
   * Caveat: TODO 50% gray inverse = 50% gray in rgb color space --> use opposite color
   */
  Color Inverse() const;


  /** @brief Checks if all rgba components are within [0, 1]. */
  bool IsValid() const;

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

/** Returns an invalid color (all components < 0). These are
 * used as special arguments, e.g. when drawing text boxes,
 * viren2d can automatically select the font color as the
 * opposite of the background color.
 */
Color InvalidColor();

//TODO group/sort the names for maintainability
namespace colors
{
  extern const Color Black;
  extern const Color White;

  extern const Color Crimson;
  extern const Color Maroon;
  extern const Color Purple;
  extern const Color Cyan;
//  extern const Color Magenta;
//  extern const Color Turquoise;
//  extern const Color Orange;
//  extern const Color Orchid;
//  extern const Color Silver;
//  extern const Color Gold;
//  extern const Color ForestGreen;
//  extern const Color TealGreen;
//  extern const Color LimeGreen;
//  extern const Color NavyBlue;
//  extern const Color Indigo;
//  extern const Color Copper;
//  extern const Color Freesia;
//  extern const Color MidnightBlue;
//  extern const Color Salmon;
//  extern const Color RoseRed;
//  extern const Color Olive;
//  extern const Color LightBlue;
//  extern const Color Lavender;
//  extern const Color Ivory;
} // namespace colors

} // namespace viren2d

#endif // __VIREN2D_COLORS_H__

