#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <map>
#include <exception>

#include <werkzeugkiste/strings/strings.h>
#include <werkzeugkiste/geometry/utils.h>

#include <viren2d/colors.h>

#include <helpers/enum.h>
#include <helpers/logging.h>

namespace wgu = werkzeugkiste::geometry;

namespace viren2d {
/** @brief Color utilities in anonymuous namespace. */
namespace {

/** @brief Saturation cast to [0, 1]. */
double cast_01(double v) {
  return saturation_cast<double>(v, 0.0, 1.0);
}


/** @brief Saturation cast to [0, 255]. */
double cast_RGB(double v) {
  return saturation_cast<double>(v, 0.0, 255.0);
}

} // namespace (anon)


std::vector<std::string> ListNamedColors() {
  SPDLOG_TRACE("ListNamedColors().");
  std::vector<std::string> lst;
  typedef ContinuousEnumIterator<NamedColor,
    NamedColor::Black, NamedColor::Invalid> NamedColorIterator;

  for (NamedColor cn: NamedColorIterator()) {
    if (cn != NamedColor::Invalid)
      lst.push_back(NamedColorToString(cn));
  }
  return lst;
}


NamedColor NamedColorFromString(const std::string &name) {
  std::string cname = werkzeugkiste::strings::Lower(name);
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
  } else if (cname.compare("invalid") == 0) {
    return NamedColor::Invalid;
  }

  std::ostringstream s;
  s << "Could not look up NamedColor corresponding to \""
    << name << "\".";
  throw std::invalid_argument(s.str());
}


std::string NamedColorToString(const NamedColor &color) {
  switch (color) {
    case NamedColor::Black: return "black";
    case NamedColor::White: return "white";
    case NamedColor::Gray: return "gray";
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
const Color Color::Invalid = Color(NamedColor::Invalid);


Color::Color(const Color &other)
  : red(other.red), green(other.green),
    blue(other.blue), alpha(other.alpha)
{}


Color::Color(const NamedColor color, double alpha) {
//  SPDLOG_TRACE("Constructing color from NamedColor({:s}),"
//               " with alpha={:.2f}.", color, alpha);

  this->alpha = alpha;
  std::ostringstream s;

  // For the following color component assignments,
  // I prefer multiple assignments (e.g. x = y = 1)
  // as I think it's easier to read than the standard
  // single assignments.
  switch (color) {
    case NamedColor::Black:
      red = green = blue = 0.0; break;

    case NamedColor::White:
      red = green = blue = 1.0; break;

    case NamedColor::Gray:  // "Grey" is just an alias
      red = green = blue = 0.5; break;

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

    case NamedColor::Invalid:
      red = green = blue = -1.0;
      this->alpha = -1.0;  // For the special "invalid" color, we also set alpha
      break;

    default:
      s << "No color code available for named color \""
        << NamedColorToString(color) << "\".";
      throw std::runtime_error(s.str());
  }
}


Color::Color(const std::string &colorspec, double alpha) {
//  SPDLOG_TRACE("Constructing color from colorspec=\"{:s}\", alpha={:.2f}.",
//               colorspec, alpha);

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
//      SPDLOG_TRACE("Overwriting alpha parameter {:.2f} with given string"
//                   " specification \"{:s}\"%.", alpha, aspec_);

      // std::stoi will throw an invalid_argument if the input can't be parsed...
      this->alpha = std::stoi(aspec_) / 100.0;
      if (this->alpha < 0.0 || this->alpha > 1.0) {
        std::ostringstream s;
        s << "Alpha in \"" << colorspec
          << "\" must be an integer within [0, 100].";
        throw std::invalid_argument(s.str());
      }

      // ... However, std::stoi will silently accept floating point
      // representations (simply return the part before the comma). We
      // explicitly require alpha in the string to be an integer:
      if (!std::all_of(aspec_.begin(), aspec_.end(), ::isdigit)) {
        std::ostringstream s;
        s << "Alpha in \"" << colorspec
          << "\" must be an integer, but it contains non-digits.";
        throw std::invalid_argument(s.str());
      }
    }
  }
}


Color::Color(std::initializer_list<double> values) {
//  SPDLOG_TRACE("Constructing color from initializer list with"
//               " {:d} values.", values.size());

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
      if (red < 0.5)
        return Color::White.WithAlpha(alpha);
      else
        return Color::Black.WithAlpha(alpha);
    } else {
      return Color(1.0 - red, 1.0 - green, 1.0 - blue, alpha);
    }
  } else {
    return *this;
  }
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
  // TODO
  // Check for being any special member
  // BEFORE the isvalid() check, because
  // all special members are invalid colors!

  std::ostringstream s;
  s << "Color";

  if (!IsValid()) { // || IsSpecialInvalid()) {
    s << "::Invalid";
  } else {
    s << "(" << ToHexString() << ")";
  }
  s << " FIXME isspecial_invalid("  << IsSpecialInvalid();//FIXME cc'tor not working?
  return s.str();

  // Alternatively as RGBa string:
//  std::ostringstream s;
//  if (IsValid()) {
//    const auto rgb = ToRGBa();
//    s << "RGBa(" << static_cast<int>(std::get<0>(rgb))
//      << ", " << static_cast<int>(std::get<1>(rgb))
//      << ", " << static_cast<int>(std::get<2>(rgb))
//      << ", " << std::fixed << std::setprecision(2)
//      << alpha << ")";
//  } else {
//    s << "RGB(-1, -1, -1)";
//  }
//  return s.str();
}


std::tuple<unsigned char, unsigned char, unsigned char, double>
Color::ToRGBa() const {
  return std::make_tuple(static_cast<unsigned char>(red * 255),
        static_cast<unsigned char>(green * 255),
        static_cast<unsigned char>(blue * 255),
        alpha);
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
  return Color(this->red, this->green,
               this->blue, alpha);
}


Color Color::operator*(double scalar) const {
  // Scale the color components (but not the alpha)
  return Color(red * scalar, green * scalar,
               blue * scalar, alpha);
}


Color &Color::operator*=(double scalar) {
  *this = (*this) * scalar;
  return *this;
}


Color Color::operator/(double scalar) const {
  return Color(red / scalar, green / scalar,
               blue / scalar, alpha);
}


Color &Color::operator/=(double scalar) {
  *this = (*this) / scalar;
  return *this;
}


Color &Color::operator+=(const Color &rhs) {
//  SPDLOG_TRACE("Adding {:s} to {:s} (with saturation cast).",
//               rhs, *this);
  red = cast_01(red + rhs.red);
  green = cast_01(green + rhs.green);
  blue = cast_01(blue + rhs.blue);
  return *this;
}


Color &Color::operator-=(const Color &rhs) {
//  SPDLOG_TRACE("Subtracting {:s} from {:s} (with saturation cast).",
//               rhs, *this);
  red = cast_01(red - rhs.red);
  green = cast_01(green - rhs.green);
  blue = cast_01(blue - rhs.blue);
  return *this;
}


bool operator==(const Color& lhs, const Color& rhs) {
  return wgu::eps_equal(lhs.red, rhs.red) && wgu::eps_equal(lhs.green, rhs.green)
      && wgu::eps_equal(lhs.blue, rhs.blue) && wgu::eps_equal(lhs.alpha, rhs.alpha);
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


Color rgba(double r, double g, double b, double alpha)
{
  return Color(cast_01(r), cast_01(g), cast_01(b),
               cast_01(alpha));
}


Color RGBa(double R, double G, double B, double alpha)
{
  return Color(cast_RGB(R/255.0), cast_RGB(G/255.0), cast_RGB(B/255.0),
               alpha);
}


Color ColorFromHexString(const std::string &webcode, double alpha) {
  SPDLOG_TRACE("ColorFromHexString(\"{:s}\", alpha={:.2f}).",
               webcode, alpha);

  const size_t len = webcode.length();
  if (len != 7 && len != 9) {
    std::ostringstream s;
    s << "Input must have a leading '#' and either 6 or 8 hex digits, "
      << "but was: \"" << webcode << "\".";
    throw std::invalid_argument(s.str());
  }

  const std::string hex = werkzeugkiste::strings::Lower(webcode);
  unsigned char rgb[3];
  for (size_t idx = 0; idx < 3; ++idx) {
    unsigned char upper = hex[idx * 2 + 1];
    upper = (upper <= '9') ? (upper - '0')
                           : (upper - 'a' + 10);

    unsigned char lower = hex[idx * 2 + 2];
    lower = (lower <= '9') ? (lower - '0')
                           : (lower - 'a' + 10);

    rgb[idx] = (upper << 4) | lower;
  }

  // Parse alpha code - if provided, it overrules the
  // optional function parameter
  if (len == 9) {
    unsigned char upper = hex[7];
    upper = (upper <= '9') ? (upper - '0')
                           : (upper - 'a' + 10);

    unsigned char lower = hex[8];
    lower = (lower <= '9') ? (lower - '0')
                           : (lower - 'a' + 10);

    alpha = static_cast<double>((upper << 4) | lower) / 255.0;
  }

  return Color(rgb[0] / 255.0, rgb[1] / 255.0,
               rgb[2] / 255.0, alpha);
}

} // namespace viren2d
