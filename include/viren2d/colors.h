#ifndef __VIREN2D_COLORS_H__
#define __VIREN2D_COLORS_H__

#include <memory>
#include <algorithm>
#include <string>
#include <vector>
#include <tuple>
#include <ostream>
#include <initializer_list>

#include <viren2d/colormaps.h>


namespace viren2d {

/// Clips the given value to the range [low, high].
template<typename _Tp>
_Tp saturation_cast(_Tp val, _Tp low, _Tp high) {
  return std::min(std::max(val, low), high);
}


/// Enum to create a Color instance from these named premixes.
enum class NamedColor : unsigned short {
  Black = 0,  ///< "Black" must be the first enum value.
  White,
  Gray,
  LightGray,

  Red,
  Green,
  Blue,

  Azure,
  Bronze,
  Brown,
  Carrot,
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
  RoseRed,
  Salmon,
  Silver,
  Spearmint,
  Tangerine,
  Taupe,
  TealGreen,
  Turquoise,
  Yellow,

  Same,   ///< "Same" must be the second-to-last element.
  Invalid ///< "Invalid" must be the last element.
};


/// Returns the string representations of all defined NamedColor values.
std::vector<std::string> ListNamedColors();


/// Converts a string representation to a premix enumeration value.
///
/// This is not case-sensitive. Any white space, hyphen, underscore
/// and the optional alpha suffix will be ignored. Thus, the following
/// would be valid inputs:
/// "forest green", "Navy Blue", "Lime-Green", "black!50", "rose-red!100"
NamedColor NamedColorFromString(const std::string &name);


/// Returns the string representation of a premix enumeration value.
std::string NamedColorToString(const NamedColor &color);


/// Overloaded ostream operator.
std::ostream &operator<<(std::ostream &os, const NamedColor &ncolor);


/// Represents a color in rgba format (rgb + alpha).
///
/// Convention:
/// * RGB (upper case letters) are in range [0, 255]
/// * rgb (lower case letters) are in range [0, 1]
/// * alpha in [0, 1] denotes the opacity from 0 fully transparent
///   to 1 fully opaque.
///
/// Details:
/// * Main reason for rgb+a is that Cairo requires rgb+a colors.
/// * The default c'tor @see Color() returns an "invalid"
///   color. This is used by the Painter to indicate
///   special handling of given color parameters.
/// * For convenience, a color can be constructed from various
///   different inputs:
///   * Via @see rgba() and @see RGBa() specification
///   * Via webcodes: "#00ff00", "#abcdefaa"
///   * Via the @see NamedColor enumeration: NamedColor::Black
///   * Via a string representation of the corresponding
///     @see NamedColor, e.g. "black", "navy-blue"
///   * The string representation can also encode the alpha
///     value, e.g. "blue!50" (for alpha = 0.5)
///   * The string representation can also encode the inverse
///      or complementary color by prepending '!' or '-':
///      "-red" == "cyan", "!blue!30" == "yellow" with alpha 0.3
class Color {
 public:
  double red;    ///< Red component within [0,1].
  double green;  ///< Green component within [0,1].
  double blue;   ///< Blue component within [0,1].
  double alpha;  ///< Alpha (opacity) component within [0,1].


  // For convenience, we provide the primary colors (and their
  // complementaries).
  static const Color White;
  static const Color Black;
  static const Color Red;
  static const Color Green;
  static const Color Blue;
  static const Color Cyan;
  static const Color Magenta;
  static const Color Yellow;

  // Also for convenience declare the special "same"
  // and "invalid" colors.
  static const Color Same;
  static const Color Invalid;


  /// Constructs an invalid color (used to indicate
  /// "special" color handling in some drawing functions).
  Color() : red(-1.0), green(-1.0), blue(-1.0), alpha(-1.0)
  {}


  /// Construct a color from the given components, clamped to [0, 1].
  Color(double red, double green, double blue, double alpha)
    : red(saturation_cast<double>(red, 0.0, 1.0)),
      green(saturation_cast<double>(green, 0.0, 1.0)),
      blue(saturation_cast<double>(blue, 0.0, 1.0)),
      alpha(saturation_cast<double>(alpha, 0.0, 1.0))
  {}


  /// Construct a color from the NamedColor enumeration.
  Color(const NamedColor color, double alpha=1.0);


  /// Create a color from the given color specification (std::string) and alpha value.
  explicit Color(const std::string &colorspec, double alpha);


  /// Create a color from the given color specification (std::string).
  explicit Color(const std::string &colorspec)
    : Color(colorspec, 1.0)
  {}


  /// Create a color from the given color specification (C string) and alpha value.
  Color(const char *colorspec, double alpha)
    : Color(std::string(colorspec), alpha)
  {}


  /// Create a color from the given color specification (C string).
  Color(const char *colorspec)
    : Color(colorspec, 1.0)
  {}


  /// Create a color from a 3- (rgb) or 4-element (rgba) initializer list.
  Color(std::initializer_list<double> values);


  /// Nothing special about the color class, so we can have
  /// the default assignment/move operators/c'tors:
  Color(const Color &other) = default;
  Color& operator=(const Color &other) = default;
  Color& operator=(Color &&) = default;
  Color(Color &&) = default;


  /// Returns the inverse/complementary color.
  ///
  /// Except for shades of gray, this returns (1.0-r, 1.0-g, 1.0-b).
  /// For gray values it will either return black or white. The alpha
  /// value will always stay the same.
  /// Why special handling of gray? Complementary colors are used in
  /// viren2d to provide good contrast/highlights - thus, having the
  /// "true" inverse (i.e. 1-r|g|b) for "medium gray" (r|g|b close
  /// to 127) would not be too useful.
  Color Inverse() const;


  /// Computes the grayscale representation (i.e. luminance) of this color.
  Color Grayscale() const;


  /// Returns a mixture of `(percentage_other) * other + (1-percentage_other) * this`.
  Color Mix(const Color &other, double percentage_other) const;


  /// Checks if all rgba components are within [0, 1].
  bool IsValid() const;


  /// Returns true if this color is equal to the special member "Invalid".
  bool IsSpecialInvalid() const;


  /// Returns true if this color is equal to the special member "Same".
  bool IsSpecialSame() const;


  /// Returns true if all rgb components almost the same (+/- the given epsilon). */
  bool IsShadeOfGray(double epsilon=0.02) const;


  /// Returns a human readable representation.
  std::string ToString() const;


  /// Returns the RGBA string representation `(R, G, B, A)`, where R, G, B
  /// in [0, 255] and A in [0, 100].
  std::string ToUInt8String() const;


  /// Returns the corresponding (R, G, B, a) tuple, where R, G, B in [0, 255]
  /// and alpha in [0, 1].
  std::tuple<unsigned char, unsigned char, unsigned char, double>
  ToRGBa() const;


  /// Returns the corresponding (H, S, V) tuple, where H in [0, 360] and
  /// S & V are in [0, 1].
  std::tuple<float, float, float> ToHSV() const;


  /// Returns the web color code, e.g. "#dcdce4ff".
  /// If the color is invalid, the hex digits will be replaced by question marks.
  std::string ToHexString() const;


  /// Returns a color with the same r,g,b components, but the given alpha.
  Color WithAlpha(double alpha) const;


  /// Scales rgba and clamps to [0, 1].
  Color operator*(double scalar) const;

  /// Scales rgba and clamps to [0, 1].
  Color &operator*=(double scalar);


  /// Divides rgba by the factor and clamps to [0, 1].
  Color operator/(double scalar) const;


  /// Divides rgba by the factor and clamps to [0, 1].
  Color &operator/=(double scalar);


  /// Adds the other's rgba and clamps the result to [0, 1].
  Color &operator+=(const Color &rhs);


  /// Subtracts the other's rgba and clamps the result to [0, 1].
  Color &operator-=(const Color &rhs);


  /// Output stream operator overload.
  friend std::ostream &operator<<(std::ostream &os, const Color &col) {
    os << col.ToString();
    return os;
  }


  /// Returns a color for the 'x', 'y', or 'z' axis.
  /// Also accepts 0, 1, or 2 to select the axis.
  static Color CoordinateAxisColor(char axis);


  /// Returns a color for the given ID (e.g. an object/class identifier).
  static Color FromObjectID(
      std::size_t id, ColorMap colormap = ColorMap::GlasbeyDark);


  /// Returns a color for the given category/object class.
  /// Useful to consistently use the same color for the same object class,
  /// *e.g.* ``car`` or ``person``.
  /// See `ListCategories` for a list of category names which are
  /// explicitly defined. For any other category name, a string
  /// hash will be computed, which is then used to lookup a
  /// corresponding color.
  static Color FromObjectCategory(
      const std::string &category, ColorMap colormap = ColorMap::GlasbeyDark);


  /// Returns a list of category names which are explicitly
  /// defined in the Color::FromCategory utility.
  static std::vector<std::string> ListObjectCategories();
};


//-------------------------------------------------  Comparison operators

bool operator==(const Color& lhs, const Color& rhs);
bool operator!=(const Color& lhs, const Color& rhs);


//-------------------------------------------------  Arithmetic operators
/// Multiplication with saturation_cast to [0,1].
Color operator*(double scalar, Color rhs);


/// Addition with saturation_cast to [0,1].
Color operator+(Color lhs, const Color& rhs);


/// Subtraction with saturation_cast to [0,1].
Color operator-(Color lhs, const Color& rhs);


//-------------------------------------------------  Convenience initialization for Color

/// Convenience wrapper to create a Color from
/// r,g,b and alpha values must in range [0,1].
Color rgba(double r, double g, double b, double alpha=1.0);


/// Convenience wrapper to create a Color from
/// RGB values in range [0,255].
/// Alpha must be in [0,1].
Color RGBa(double R, double G, double B, double alpha=1.0);


/// Creates a Color from the given webcode, e.g. "#abcdef".
///
/// If the hexstring/webcode has 8 digits, its alpha specification
/// has precedence over the given "alpha" parameter.
Color ColorFromHexString(const std::string &webcode, double alpha=1.0);


/// Identity function to implement a linear color transition
/// in `viren2d::Painter::DrawTrajectory`.
double ColorFadeOutLinear(double progress);


/// Returns a quadratic factor for the color transition
/// in `viren2d::Painter::DrawTrajectory`.
double ColorFadeOutQuadratic(double progress);


/// Returns a logarithmic factor for the color transition
/// in `viren2d::Painter::DrawTrajectory`.
double ColorFadeOutLogarithmic(double progress);

} // namespace viren2d

#endif // __VIREN2D_COLORS_H__

