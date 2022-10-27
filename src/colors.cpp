#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <map>
#include <exception>
#include <utility>
#include <functional>  // std::hash
#include <cstdlib>

#include <werkzeugkiste/strings/strings.h>
#include <werkzeugkiste/geometry/utils.h>
#include <werkzeugkiste/container/sort.h>

#include <viren2d/colors.h>

#include <helpers/enum.h>
#include <helpers/logging.h>
#include <helpers/colormaps_helpers.h>
#include <helpers/color_conversion.h>


namespace wzkc = werkzeugkiste::container;
namespace wzkg = werkzeugkiste::geometry;
namespace wzks = werkzeugkiste::strings;


namespace viren2d {
/// Separate namespace for color utilities.
namespace helpers {

/// Saturation cast to [0, 1]. */
double cast_01(double v) {
  if ((v < 0.0) || (v > 1.0)) {
    SPDLOG_WARN("Clipping value {:f} to [0, 1].", v);
  }
  return saturation_cast<double>(v, 0.0, 1.0);
}


/// Saturation cast to [0, 255].
double cast_RGB(double v) {
  return saturation_cast<double>(v, 0.0, 255.0);
}


/// Mapping COCO category names (+ some widely used aliases)
/// to their object class IDs.
/// This is used to implement `FromCategory`.
const std::map<std::string, std::size_t> kCategoryIDMapping {
  {"background", 0}, {"person", 1}, {"bicycle", 2}, {"car", 3}, {"motorcycle", 4},
  {"airplane", 5}, {"bus", 6}, {"train", 7}, {"truck", 8}, {"boat", 9},
  {"traffic-light", 10}, {"fire-hydrant", 11}, {"stop-sign", 12},
  {"parking-meter", 13}, {"bench", 14}, {"bird", 15}, {"cat", 16}, {"dog", 17},
  {"horse", 18}, {"sheep", 19}, {"cow", 20}, {"elephant", 21}, {"bear", 22},
  {"zebra", 23}, {"giraffe", 24}, {"backpack", 25}, {"umbrella", 26},
  {"handbag", 27}, {"tie", 28}, {"suitcase", 29}, {"frisbee", 30}, {"skis", 31},
  {"snowboard", 32}, {"sports-ball", 33}, {"kite", 34}, {"baseball-bat", 35},
  {"baseball-glove", 36}, {"skateboard", 37}, {"surfboard", 38}, {"tennis-racket", 39},
  {"bottle", 40}, {"wine-glass", 41}, {"cup", 42}, {"fork", 43}, {"knife", 44},
  {"spoon", 45}, {"bowl", 46}, {"banana", 47}, {"apple", 48}, {"sandwich", 49},
  {"orange", 50}, {"broccoli", 51}, {"carrot", 52}, {"hot-dog", 53}, {"pizza", 54},
  {"donut", 55}, {"cake", 56}, {"chair", 57}, {"couch", 58}, {"potted-plant", 59},
  {"bed", 60}, {"dining-table", 61}, {"toilet", 62}, {"tv", 63}, {"laptop", 64},
  {"mouse", 65}, {"remote", 66}, {"keyboard", 67}, {"cell-phone", 68}, {"microwave", 69},
  {"oven", 70}, {"toaster", 71}, {"sink", 72}, {"refrigerator", 73}, {"book", 74},
  {"clock", 75}, {"vase", 76}, {"scissors", 77}, {"teddy-bear", 78}, {"hair-drier", 79},
  {"toothbrush", 80},
  // Aliases:
  {"human", 1}, {"pedestrian", 1},
  {"vehicle", 3},
  {"smartphone", 68}
};

} // namespace helpers


std::vector<std::string> ListNamedColors() {
  SPDLOG_TRACE("ListNamedColors().");
  std::vector<std::string> lst;
  typedef ContinuousEnumIterator<NamedColor,
    NamedColor::Black, NamedColor::Invalid> NamedColorIterator;

  for (NamedColor cn: NamedColorIterator()) {
    if ((cn != NamedColor::Invalid)
        && (cn != NamedColor::Same)) {
      lst.push_back(NamedColorToString(cn));
    }
  }
  return lst;
}


NamedColor NamedColorFromString(const std::string &name) {
  std::string cname = wzks::Lower(name);
  cname.erase(std::remove_if(cname.begin(), cname.end(), [](char ch) -> bool {
      return ::isspace(ch) || (ch == '-') || (ch == '_');
    }), cname.end());

  // Ignore potential alpha specification, e.g. "blue!30"
  size_t pos = cname.find('!');
  if (pos != std::string::npos) {
    cname = cname.substr(0, pos);
  }
  SPDLOG_TRACE("NamedColorFromString(\"{:s}\"), canonical"
               " name=\"{:s}\".", name, cname);

  if (cname.compare("black") == 0) {
    return NamedColor::Black;
  } else if (cname.compare("white") == 0) {
    return NamedColor::White;
  } else if ((cname.compare("gray") == 0)
             || (cname.compare("grey") == 0)) {
    return NamedColor::Gray;
  } else if ((cname.compare("lightgray") == 0)
             || (cname.compare("lightgrey") == 0)) {
    return NamedColor::LightGray;
  } else if (cname.compare("red") == 0) {
    return NamedColor::Red;
  } else if (cname.compare("green") == 0) {
    return NamedColor::Green;
  } else if (cname.compare("blue") == 0) {
    return NamedColor::Blue;
  } else if (cname.compare("azure") == 0) {
    return NamedColor::Azure;
  } else if (cname.compare("bronze") == 0) {
    return NamedColor::Bronze;
  } else if (cname.compare("brown") == 0) {
    return NamedColor::Brown;
  } else if (cname.compare("carrot") == 0) {
    return NamedColor::Carrot;
  } else if (cname.compare("copper") == 0) {
    return NamedColor::Copper;
  } else if (cname.compare("crimson") == 0) {
    return NamedColor::Crimson;
  } else if (cname.compare("cyan") == 0) {
    return NamedColor::Cyan;
  } else if (cname.compare("forestgreen") == 0) {
    return NamedColor::ForestGreen;
  } else if (cname.compare("freesia") == 0) {
    return NamedColor::Freesia;
  } else if (cname.compare("gold") == 0) {
    return NamedColor::Gold;
  } else if (cname.compare("indigo") == 0) {
    return NamedColor::Indigo;
  } else if (cname.compare("ivory") == 0) {
    return NamedColor::Ivory;
  } else if (cname.compare("lavender") == 0) {
    return NamedColor::Lavender;
  } else if (cname.compare("lightblue") == 0) {
    return NamedColor::LightBlue;
  } else if (cname.compare("limegreen") == 0) {
    return NamedColor::LimeGreen;
  } else if (cname.compare("maroon") == 0) {
    return NamedColor::Maroon;
  } else if (cname.compare("magenta") == 0) {
    return NamedColor::Magenta;
  } else if (cname.compare("midnightblue") == 0) {
    return NamedColor::MidnightBlue;
  } else if (cname.compare("navyblue") == 0) {
    return NamedColor::NavyBlue;
  } else if (cname.compare("olive") == 0) {
    return NamedColor::Olive;
  } else if (cname.compare("orange") == 0) {
    return NamedColor::Orange;
  } else if (cname.compare("orchid") == 0) {
    return NamedColor::Orchid;
  } else if (cname.compare("purple") == 0) {
    return NamedColor::Purple;
  } else if (cname.compare("rosered") == 0) {
    return NamedColor::RoseRed;
  } else if (cname.compare("salmon") == 0) {
    return NamedColor::Salmon;
  } else if (cname.compare("silver") == 0) {
    return NamedColor::Silver;
  } else if (cname.compare("spearmint") == 0) {
    return NamedColor::Spearmint;
  } else if (cname.compare("tangerine") == 0) {
    return NamedColor::Tangerine;
  } else if (cname.compare("taupe") == 0) {
    return NamedColor::Taupe;
  } else if (cname.compare("tealgreen") == 0) {
    return NamedColor::TealGreen;
  } else if (cname.compare("turquoise") == 0) {
    return NamedColor::Turquoise;
  } else if (cname.compare("yellow") == 0) {
    return NamedColor::Yellow;
  } else if (cname.compare("same") == 0) {
    return NamedColor::Same;
  } else if ((cname.compare("invalid") == 0)
             || (cname.compare("none") == 0)) {
    return NamedColor::Invalid;
  }

  std::string s("Could not look up NamedColor corresponding to \"");
  s += name;
  s += "\"!";
  throw std::invalid_argument(s);
}


std::string NamedColorToString(const NamedColor &color) {
  switch (color) {
    case NamedColor::Black: return "black";
    case NamedColor::White: return "white";
    case NamedColor::Gray: return "gray";
    case NamedColor::LightGray: return "light-gray";

    case NamedColor::Red: return "red";
    case NamedColor::Green: return "green";
    case NamedColor::Blue: return "blue";

    case NamedColor::Azure: return "azure";
    case NamedColor::Bronze: return "bronze";
    case NamedColor::Brown: return "brown";
    case NamedColor::Carrot: return "carrot";
    case NamedColor::Copper: return "copper";
    case NamedColor::Crimson: return "crimson";
    case NamedColor::Cyan: return "cyan";
    case NamedColor::ForestGreen: return "forest-green";
    case NamedColor::Freesia: return "freesia";
    case NamedColor::Gold: return "gold";
    case NamedColor::Indigo: return "indigo";
    case NamedColor::Ivory: return "ivory";
    case NamedColor::Lavender: return "lavender";
    case NamedColor::LightBlue: return "light-blue";
    case NamedColor::LimeGreen: return "lime-green";
    case NamedColor::Maroon: return "maroon";
    case NamedColor::Magenta: return "magenta";
    case NamedColor::MidnightBlue: return "midnight-blue";
    case NamedColor::NavyBlue: return "navy-blue";
    case NamedColor::Olive: return "olive";
    case NamedColor::Orange: return "orange";
    case NamedColor::Orchid: return "orchid";
    case NamedColor::Purple: return "purple";
    case NamedColor::RoseRed: return "rose-red";
    case NamedColor::Salmon: return "salmon";
    case NamedColor::Silver: return "silver";
    case NamedColor::Spearmint: return "spearmint";
    case NamedColor::Tangerine: return "tangerine";
    case NamedColor::Taupe: return "taupe";
    case NamedColor::TealGreen: return "teal-green";
    case NamedColor::Turquoise: return "turquoise";
    case NamedColor::Yellow: return "yellow";

    case NamedColor::Same: return "same";
    case NamedColor::Invalid: return "invalid";

    default: {
      std::ostringstream s;
      s << "No string representation available for this NamedColor ("
        << static_cast<unsigned short>(color) << ")";
      throw std::runtime_error(s.str());
    }
  }
}

std::ostream &operator<<(std::ostream &os, const NamedColor &ncolor) {
  os << NamedColorToString(ncolor);
  return os;
}


const Color Color::Black   = Color(NamedColor::Black);
const Color Color::White   = Color(NamedColor::White);
const Color Color::Red     = Color(NamedColor::Red);
const Color Color::Green   = Color(NamedColor::Green);
const Color Color::Blue    = Color(NamedColor::Blue);
const Color Color::Cyan    = Color(NamedColor::Cyan);
const Color Color::Magenta = Color(NamedColor::Magenta);
const Color Color::Yellow  = Color(NamedColor::Yellow);
const Color Color::Same =    Color(NamedColor::Same);
const Color Color::Invalid = Color(NamedColor::Invalid);


Color::Color(const NamedColor color, double alpha) {
  this->alpha = alpha;

  // For the following color component assignments,
  // I prefer multiple assignments (e.g. x = y = 1)
  // for readability.
  switch (color) {
    case NamedColor::Black:
      red = green = blue = 0.0; break;

    case NamedColor::White:
      red = green = blue = 1.0; break;

    case NamedColor::Gray:
      red = green = blue = 0.5; break;

    case NamedColor::LightGray:
      red = green = blue = 0.78; break;

    case NamedColor::Red:
      red = 1.0; green = blue = 0.0; break;

    case NamedColor::Green:
      green = 1.0; red = blue = 0.0; break;

    case NamedColor::Blue:
      blue = 1.0; red = green = 0.0; break;

    case NamedColor::Azure:
      red = 0.0; green = 0.5; blue = 1.0; break;

    case NamedColor::Bronze:
      red = 0.8; green = 0.5; blue = 0.20; break;

    case NamedColor::Brown:
      red = 0.53; green = 0.33; blue = 0.04; break;

    case NamedColor::Carrot:
      red = 0.93; green = 0.57; blue = 0.13; break;

    case NamedColor::Copper:
      red = 0.72; green = 0.45; blue = 0.2; break;

    case NamedColor::Crimson:
      red = 0.6; green = blue = 0.0; break;

    case NamedColor::Cyan:
      red = 0.0; green = blue = 1.0; break;

    case NamedColor::ForestGreen:
      red = 0.13; green = 0.55; blue = 0.13; break;

    case NamedColor::Freesia:
      red = 0.97; green = 0.77; blue = 0.14; break;

    case NamedColor::Gold:
      red = 1.0; green = 0.84; blue = 0.0; break;

    case NamedColor::Indigo:
      red = 0.3; green = 0.0; blue = 0.51; break;

    case NamedColor::Ivory:
      red = 1.0; green = 1.0; blue = 0.94; break;

    case NamedColor::Lavender:
      red = 0.9; green = 0.9; blue = 0.98; break;

    case NamedColor::LightBlue:
      red = 0.68; green = 0.85; blue = 0.9; break;

    case NamedColor::LimeGreen:
      red = 0.2; green = 0.8; blue = 0.2; break;

    case NamedColor::Magenta:
      red = 1.0; green = 0.0; blue = 1.0; break;

    case NamedColor::Maroon:
      red = 0.5; green = 0.0; blue = 0.0; break;

    case NamedColor::MidnightBlue:
      red = 0.1; green = 0.1; blue = 0.44; break;

    case NamedColor::NavyBlue:
      red = 0.0; green = 0.0; blue = 0.5; break;

    case NamedColor::Olive:
      red = 0.5; green = 0.5; blue = 0.0; break;

    case NamedColor::Orange:
      red = 1.0; green = 0.65; blue = 0.0; break;

    case NamedColor::Orchid:
      red = 0.86; green = 0.44; blue = 0.84; break;

    case NamedColor::Purple:
      red = 0.63; green = 0.13; blue = 0.94; break;

    case NamedColor::RoseRed:
      red = 1.0; green = 0.01; blue = 0.24; break;

    case NamedColor::Salmon:
      red = 0.98; green = 0.5; blue = 0.45; break;

    case NamedColor::Silver:
      red = 0.75; green = 0.75; blue = 0.75; break;

    case NamedColor::Spearmint:
      red = 0.27; green = 0.69; blue = 0.55; break;

    case NamedColor::Tangerine:
      red = 0.95; green = 0.52; blue = 0.0; break;

    case NamedColor::Taupe:
      red = 0.28; green = 0.24; blue = 0.20; break;

    case NamedColor::TealGreen:
      red = 0.0; green = 0.43; blue = 0.36; break;

    case NamedColor::Turquoise:
      red = 0.19; green = 0.84; blue = 0.78; break;

    case NamedColor::Yellow:
      red = green = 1.0; blue = 0.0; break;

    case NamedColor::Same:
      red = green = blue = -0.5;
      break;

    case NamedColor::Invalid:
      red = green = blue = -1.0;
      this->alpha = -1.0;  // For the special "invalid" color, we also set alpha
      break;

    default: {
        std::string s("No color code available for named color \"");
        s += NamedColorToString(color);
        s += "\"!";
        throw std::runtime_error(s);
      }
  }
}


Color::Color(const std::string &colorspec, double alpha) {
  if (colorspec.length() > 1 && colorspec[0] == '#') {
    *this = ColorFromHexString(colorspec, alpha);
  } else {
    // Check if we need special handling later on
    const bool invert = colorspec.length() > 1 &&
        ((colorspec[0] == '!') || (colorspec[0] == '-'));
    const std::string &cspec_ = invert ? colorspec.substr(1)
                                       : colorspec;
    // Get the "base" color from the string and invert if needed
    *this = Color(NamedColorFromString(cspec_), alpha);
    if (invert) {
      *this = Inverse();
    }

    // Check if the colorspec string contained an alpha specification.
    size_t pos = cspec_.find('!');
    if (pos != std::string::npos) {
      // If so, the "string alpha" (which must be an integer in [0, 100])
      // will overwrite the constructor's alpha parameter
      const std::string aspec_ = cspec_.substr(pos + 1);

      // std::stoi will throw an invalid_argument if the input can't be parsed...
      this->alpha = std::stoi(aspec_) / 100.0;
      if (this->alpha < 0.0 || this->alpha > 1.0) {
        std::string s("Alpha in \"");
        s += colorspec;
        s += "\" must be an integer within [0, 100].";
        throw std::invalid_argument(s);
      }

      // ... However, std::stoi will silently accept floating point
      // representations (simply return the part before the comma). We
      // explicitly require alpha in the string to be an integer:
      if (!std::all_of(aspec_.begin(), aspec_.end(), ::isdigit)) {
        std::string s("Alpha in \"");
        s += colorspec;
        s += "\" must be an integer, but it contains non-digits.";
        throw std::invalid_argument(s);
      }
    }
  }
}


Color::Color(std::initializer_list<double> values) {
  if (values.size() == 0) {
    *this = Color();
  } else if (values.size() >= 3) {
    const auto val = values.begin();
    red = val[0];
    green = val[1];
    blue = val[2];

    if (values.size() > 3) {
      alpha = val[3];
    } else {
      alpha = 1.0;
    }
  } else {
    std::ostringstream s;
    s << "Color c'tor requires 0, 3 or 4 elements in initializer_list, "
      << "but got " << values.size() << ".";
    throw std::invalid_argument(s.str());
  }
}


Color Color::Inverse() const {
  SPDLOG_TRACE("Computing the complementary color"
               " to {:s}.", *this);

  if (IsValid()) {
    if (IsShadeOfGray()) {
      if (red < 0.5) {
        return Color::White.WithAlpha(alpha);
      } else {
        return Color::Black.WithAlpha(alpha);
      }
    } else {
      return Color(1.0 - red, 1.0 - green,
                   1.0 - blue, alpha);
    }
  } else {
    return *this;
  }
}


Color Color::ToGray() const {
  const double luminance = helpers::CvtHelperRGB2Gray(red, green, blue);
  return Color(luminance, luminance, luminance, alpha);
}


Color Color::Mix(const Color &other, double percentage_other) const {
  percentage_other = saturation_cast<double>(percentage_other, 0.0, 1.0);
  return (percentage_other * other) + ((1.0 - percentage_other) * (*this));
}


bool Color::IsValid() const {
  if (red < 0.0 || red > 1.0)
    return false;
  if (green < 0.0 || green > 1.0)
    return false;
  if (blue < 0.0 || blue > 1.0)
    return false;
  if (alpha < 0.0 || alpha > 1.0)
    return false;
  return true;
}


bool Color::IsSpecialInvalid() const {
  return *this == Invalid;
}


bool Color::IsSpecialSame() const {
  // Check color components but not alpha.
  return wzkg::eps_equal(red, Same.red)
      && wzkg::eps_equal(green, Same.green)
      && wzkg::eps_equal(blue, Same.blue);
}


bool Color::IsShadeOfGray(double epsilon) const {
  // No need to check red vs blue thanks to transitivity
  if ((red < (green - epsilon)) || (red > (green + epsilon))
      || (green < (blue - epsilon)) || (green > (blue + epsilon))) {
    SPDLOG_TRACE("{:s} is not a shade of gray (at eps={:.3f}).",
                 *this, epsilon);
    return false;
  } else {
    SPDLOG_TRACE("{:s} is shade of gray (at eps={:.3f}).", *this, epsilon);
    return true;
  }
}

std::string Color::ToString() const {
  // Note that we have to check whether the
  // color is a "special" member BEFORE the
  // IsValid() check, because **all** special
  // members are invalid colors!
  std::ostringstream s;

  if (IsSpecialSame()) {
    s << "Color::Same(a=" << std::fixed
      << std::setprecision(2) << alpha
      << ")";
  } else {
    if (!IsValid()) {
      s << "Color::Invalid";
    } else {
      s << ToHexString();
    }
  }
  return s.str();
}


std::string Color::ToUInt8String(bool fixed_width) const {
  std::ostringstream s;
  s << '(';

  if (IsSpecialSame()) {
    s << "Same, a=" << std::fixed
      << std::setprecision(2) << alpha;
  } else {
    if (!IsValid()) {
      s << "Invalid: ";
    }

    if (fixed_width) {
      s << std::setw(3)
        << static_cast<int>(255 * red) << ", "
        << static_cast<int>(255 * green) << ", "
        << static_cast<int>(255 * blue) << ", "
        << static_cast<int>(100 * alpha) << ')';
    } else {
      s << static_cast<int>(255 * red) << ", "
        << static_cast<int>(255 * green) << ", "
        << static_cast<int>(255 * blue) << ", "
        << static_cast<int>(100 * alpha) << ')';
    }
  }
  return s.str();
}


std::tuple<unsigned char, unsigned char, unsigned char, double>
Color::ToRGBa() const {
  return std::make_tuple(static_cast<unsigned char>(red * 255),
        static_cast<unsigned char>(green * 255),
        static_cast<unsigned char>(blue * 255),
        alpha);
}


std::tuple<float, float, float> Color::ToHSV() const {
  return helpers::CvtHelperRGB2HSV(red, green, blue);
}


std::string Color::ToHexString() const {
  if (!IsValid())
    return std::string("#????????");

  std::string webcode("#00000000");

  // Mapping from [0,15] to corresponding hex code character
  std::map<unsigned char, char> hex2char {
    {0, '0'}, {1, '1'}, {2, '2'}, {3, '3'}, {4, '4'},
    {5, '5'}, {6, '6'}, {7, '7'}, {8, '8'}, {9, '9'},
    {10, 'a'}, {11, 'b'}, {12, 'c'}, {13, 'd'},
    {14, 'e'}, {15, 'f'}
  };

  // RGB is easier to work with
  auto RGBa = ToRGBa();

  // For now, tuple elements can't be accessed
  // in a loop. So bear with the following copy&paste.
  //
  // Off-topic: Compiler optimizations never cease to
  // impress me, e.g. check how these two lines are
  // optimized (at least on x86, very likely others too)
  // to reuse the remainder: https://stackoverflow.com/a/7070598/400948
  unsigned char div = static_cast<unsigned char>(std::get<0>(RGBa) / 16);
  unsigned char rem = static_cast<unsigned char>(std::get<0>(RGBa) % 16);
  webcode[1] = hex2char[div];
  webcode[2] = hex2char[rem];

  div = static_cast<unsigned char>(std::get<1>(RGBa) / 16);
  rem = static_cast<unsigned char>(std::get<1>(RGBa) % 16);
  webcode[3] = hex2char[div];
  webcode[4] = hex2char[rem];

  div = static_cast<unsigned char>(std::get<2>(RGBa) / 16);
  rem = static_cast<unsigned char>(std::get<2>(RGBa) % 16);
  webcode[5] = hex2char[div];
  webcode[6] = hex2char[rem];

  // Scale alpha to [0, 255]
  auto a = static_cast<unsigned char>(alpha * 255);
  div = static_cast<unsigned char>(a / 16);
  rem = static_cast<unsigned char>(a % 16);
  webcode[7] = hex2char[div];
  webcode[8] = hex2char[rem];

  return webcode;
}


Color Color::WithAlpha(double alpha) const {
  // Explicitly use the copy c'tor, because we
  // want to avoid the saturation cast of the
  // default c'tor. This way we can use the special
  // "Same" color, but with a different alpha,
  // i.e. Color::Same.WithAlpha(0.3):
  Color copy(*this);
  copy.alpha = helpers::cast_01(alpha);
  return copy;
}


Color Color::operator*(double scalar) const {
  return Color(
        red * scalar, green * scalar,
        blue * scalar, alpha * scalar);
}


Color &Color::operator*=(double scalar) {
  *this = (*this) * scalar;
  return *this;
}


Color Color::operator/(double scalar) const {
  return Color(
        red / scalar, green / scalar,
        blue / scalar, alpha / scalar);
}


Color &Color::operator/=(double scalar) {
  *this = (*this) / scalar;
  return *this;
}


Color &Color::operator+=(const Color &rhs) {
  red = helpers::cast_01(red + rhs.red);
  green = helpers::cast_01(green + rhs.green);
  blue = helpers::cast_01(blue + rhs.blue);
  alpha = helpers::cast_01(alpha + rhs.alpha);
  return *this;
}


Color &Color::operator-=(const Color &rhs) {
  red = helpers::cast_01(red - rhs.red);
  green = helpers::cast_01(green - rhs.green);
  blue = helpers::cast_01(blue - rhs.blue);
  alpha = helpers::cast_01(alpha - rhs.alpha);
  return *this;
}


Color Color::CoordinateAxisColor(char axis) {
  switch (axis) {
    case 0:
    case 'x':
    case 'X':
      return Color(0.94, 0.13, 0.15, 1.0);

    case 1:
    case 'y':
    case 'Y':
      return Color(0.19, 0.63, 0.30, 1.0);

    case 2:
    case 'z':
    case 'Z':
      return Color(0.09, 0.44, 0.72, 1.0);

    default:
      return Color::Magenta;
  }
}


Color Color::FromObjectID(std::size_t id, ColorMap colormap) {
  helpers::RGBColor col = helpers::GetCategoryColor(id, colormap);
  return RGBa(col.red, col.green, col.blue);
}


Color Color::FromObjectCategory(const std::string &category, ColorMap colormap) {
  std::string slug = wzks::Replace(wzks::Lower(category), ' ', '-');
  const auto it = helpers::kCategoryIDMapping.find(slug);

  if (it != helpers::kCategoryIDMapping.end()) {
    return FromObjectID(it->second, colormap);
  } else {
    std::hash<std::string> string_hash;
    return FromObjectID(string_hash(slug), colormap);
  }
}


std::vector<std::string> Color::ListObjectCategories() {
  std::vector<std::string> categories = wzkc::GetMapKeys(helpers::kCategoryIDMapping);
  // The default comparator results in ascending order:
  std::sort(categories.begin(), categories.end());
  return categories;
}


bool operator==(const Color& lhs, const Color& rhs) {
  return wzkg::eps_equal(lhs.red, rhs.red)
      && wzkg::eps_equal(lhs.green, rhs.green)
      && wzkg::eps_equal(lhs.blue, rhs.blue)
      && wzkg::eps_equal(lhs.alpha, rhs.alpha);
}


bool operator!=(const Color& lhs, const Color& rhs) {
  return !(lhs == rhs);
}


Color operator*(double scalar, Color rhs) {
  return rhs * scalar;
}


Color operator+(Color lhs, const Color& rhs) {
  lhs += rhs;
  return lhs;
}


Color operator-(Color lhs, const Color& rhs) {
  lhs -= rhs;
  return lhs;
}


Color rgba(double r, double g, double b, double alpha) {
  return Color(r, g, b, alpha);
}


Color RGBa(double R, double G, double B, double alpha) {
  return Color(R/255.0, G/255.0, B/255.0, alpha);
}


Color ColorFromHexString(const std::string &webcode, double alpha) {
  SPDLOG_TRACE(
        "ColorFromHexString(\"{:s}\", alpha={:.2f}).", webcode, alpha);

  const size_t len = webcode.length();
  if (len != 7 && len != 9) {
    std::string s(
          "Input must have a leading '#' and either "
          "6 or 8 hex digits, but was: \"");
    s += webcode;
    s += "\"!";
    throw std::invalid_argument(s);
  }

  const std::string hex = wzks::Lower(webcode);
  unsigned char rgb[3];
  for (size_t idx = 0; idx < 3; ++idx) {
    unsigned char upper = hex[idx * 2 + 1];
    upper = (upper <= '9') ? (upper - '0') : (upper - 'a' + 10);

    unsigned char lower = hex[idx * 2 + 2];
    lower = (lower <= '9') ? (lower - '0') : (lower - 'a' + 10);

    rgb[idx] = (upper << 4) | lower;
  }

  // Parse alpha code (overrules the optional function parameter if provided)
  if (len == 9) {
    unsigned char upper = hex[7];
    upper = (upper <= '9') ? (upper - '0') : (upper - 'a' + 10);

    unsigned char lower = hex[8];
    lower = (lower <= '9') ? (lower - '0') : (lower - 'a' + 10);

    alpha = static_cast<double>((upper << 4) | lower) / 255.0;
  }

  return Color(rgb[0] / 255.0, rgb[1] / 255.0, rgb[2] / 255.0, alpha);
}


double ColorFadeOutLinear(double progress) {
  return progress;
}


double ColorFadeOutQuadratic(double progress) {
  return progress * progress;
}


double ColorFadeOutLogarithmic(double progress) {
  // y = 0.5 at 25% progress
  return std::log10(progress * 9.0 + 1);
}


void SetCustomColorMap(
    ColorMap colormap, const std::vector<Color> &colors) {
  std::vector<helpers::RGBColor> rgb_colors;
  for (const auto &c : colors) {
    rgb_colors.push_back(
          helpers::RGBColor(
            static_cast<unsigned char>(255 * c.red),
            static_cast<unsigned char>(255 * c.green),
            static_cast<unsigned char>(255 * c.blue)));
  }

  helpers::SetUserDefinedColorMap(colormap, rgb_colors);
}


/// Returns the colors for the specified color map.
std::vector<Color> GetColorMapColors(ColorMap colormap) {
  std::pair<const helpers::RGBColor *, std::size_t> map =
      helpers::GetColorMap(colormap);

  std::vector<Color> colors;
  colors.reserve(map.second);

  for (std::size_t i = 0; i < map.second; ++i) {
    colors.push_back(
          RGBa(map.first[i].red, map.first[i].green, map.first[i].blue));
  }

  return colors;
}


std::vector<Color> ColorizeScalars(
    const std::vector<double> &values, ColorMap colormap,
    double limit_low, double limit_high, int bins) {
  std::vector<Color> colors(values.size());

  if (bins < 2) {
    std::ostringstream s;
    s << "Number of bins for `ColorizeValues` must be > 1, but got: "
      << bins << '!';
    throw std::invalid_argument(s.str());
  }

  //FIXME should we support this here? --> implement min/max in werkzeugkiste!!
//  if (std::isinf(limit_low) || std::isinf(limit_high)
//      || std::isnan(limit_low) ||std::isnan(limit_high)) {
//    data.MinMaxLocation(&limit_low, &limit_high);
//  }

  if (limit_high <= limit_low) {
    std::ostringstream s;
    s << "Invalid colorization limits [" << std::fixed
      << std::setprecision(2) << limit_low
      << ", " << limit_high << "]!";
    throw std::invalid_argument(s.str());
  }

  std::pair<const helpers::RGBColor *, std::size_t> map = helpers::GetColorMap(
        colormap);
  bins = std::min(bins, static_cast<int>(map.second));

  const int map_bins = map.second - 1;
  const double map_idx_factor = static_cast<double>(map_bins) / (bins - 1);
  const double interval = (limit_high - limit_low) / bins;

  for (std::size_t idx = 0; idx < values.size(); ++idx) {
    const double value = std::max(
          limit_low, std::min(limit_high, values[idx]));
    const int bin = std::max(0, std::min(map_bins, static_cast<int>(
              map_idx_factor * std::floor((value - limit_low) / interval))));
    colors[idx] = RGBa(
          map.first[bin].red, map.first[bin].green,
          map.first[bin].blue, 1.0);
  }

  return colors;
}


} // namespace viren2d
