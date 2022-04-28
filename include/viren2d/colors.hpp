#ifndef __VIREN2D_COLORS_H__
#define __VIREN2D_COLORS_H__

#include <memory>
#include <algorithm>
#include <string>
#include <tuple>
#include <ostream>

//TODO colormaps & pseudocoloring - for v2?


namespace viren2d {

/** @brief Clips the given value to the range [low, high]. */
template<typename _Tp>
_Tp saturation_cast(_Tp val, _Tp low, _Tp high) {
  return std::min(std::max(val, low), high);
}


//-------------------------------------------------  Color


enum class NamedColor : unsigned short {
  Black = 0,
  White,
  Gray,
  Grey = Gray,  // alias

  Red,
  Green,
  Blue,

  Copper,
  Crimson,
  Cyan,
  ForestGreen,
  Freesia,
  Gold,
  Indigo,
  Ivory,
  Lavender,
  LightBlue,
  LimeGreen,
  Maroon,
  Magenta,
  MidnightBlue,
  NavyBlue,
  Olive,
  Orange,
  Orchid,
  Purple,
  Salmon,
  Silver,
  TealGreen,
  Turquoise,
  RoseRed,

  Invalid /**< "Invalid" must be the last element (as it denotes the end of the enum definition internally). */
};
// TODO for tests we could try iterating over the enum values like here: https://stackoverflow.com/a/31836401

NamedColor NamedColorFromString(const std::string &name);


std::string NamedColorToString(NamedColor color);


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
class Color {
 public:
  double red;   /**< Red component within [0,1]. */
  double green; /**< Green component within [0,1]. */
  double blue;  /**< Blue component within [0,1]. */
  double alpha; /**< Alpha (opacity) component within [0,1]. */

  //TODO initializes an invalid color
  //TESTED
  Color() : red(-1.0), green(-1.0), blue(-1.0), alpha(1.0)
  {}

  // TODO initialize from r,g,b clamped to [0,1]
  //TESTED
  Color(double red, double green, double blue, double alpha)
    : red(saturation_cast<double>(red, 0.0, 1.0)),
      green(saturation_cast<double>(green, 0.0, 1.0)),
      blue(saturation_cast<double>(blue, 0.0, 1.0)),
      alpha(saturation_cast<double>(alpha, 0.0, 1.0))
  {}

  // TODO initialize from different color with different transparency
  //TESTED
  Color(const Color &other, double alpha)
    : red(other.red), green(other.green), blue(other.blue),
      alpha(saturation_cast<double>(alpha, 0.0, 1.0))
  {}

  // TODO initialize from a color name
  //TESTED
  Color(const NamedColor color, double alpha=1.0);

  // TODO doc
  //TESTED
  // Uses constructor delegation
  //TODO nice-to-have: init from colorname (black, maroon, ...) OR from hex code (if strstartswith('#'))
  Color(const std::string &color_name, double alpha=1.0)
    : Color(NamedColorFromString(color_name), alpha)
  {}

  /**
   * @brief Returns the inverse color, i.e. (1.0-r, 1.0-g, 1.0-b). Alpha value stays the same.
   *
   * Caveat: TODO 50% gray inverse = 50% gray in rgb color space --> use opposite color
   */
  Color Inverse() const; //TODO if invalid --> return invalid!


  //TESTED
  /** @brief Checks if all rgba components are within [0, 1]. */
  bool IsValid() const;

  //TODO
  std::string ToString() const;

  /** @brief Returns the corresponding (R, G, B, a) tuple, where R, G, B in [0, 255] and alpha in [0, 1]. */
  std::tuple<unsigned char, unsigned char, unsigned char, double>
  ToRGBA() const;


  /** @brief Returns the web color code, e.g. "#dcdce4" (alpha value is ignored). */
  std::string ToHexString() const;

  friend std::ostream &operator<<(std::ostream &os, const Color &col) {
    os << col.ToString();
    return os;
  }
  //TODO * scalar
};


//-------------------------------------------------  Comparison operators

bool operator==(const Color& lhs, const Color& rhs);
bool operator!=(const Color& lhs, const Color& rhs);


//-------------------------------------------------  Convenience initialization for Color

/** Convenience wrapper to initialize @see Color from rgb values in range [0,1]. */
Color rgba(double r, double g, double b, double alpha=1.0);

/** Convenience wrapper to initialize @see Color from RGB values in range [0,255]. Alpha must be in [0,1]. */
Color RGBA(double R, double G, double B, double alpha=1.0);

/** Returns an invalid color (all components < 0). These are
 * used as special arguments, e.g. when drawing text boxes,
 * viren2d can automatically select the font color as the
 * opposite of the background color.
 */
Color InvalidColor();

} // namespace viren2d

#endif // __VIREN2D_COLORS_H__

