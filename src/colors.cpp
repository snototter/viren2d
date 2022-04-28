#include <sstream>
#include <iomanip>
#include <map>
#include <exception>
#include <algorithm>

#include <viren2d/colors.hpp>
#include <viren2d/math.hpp>
#include <viren2d/string_utils.hpp>

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


NamedColor NamedColorFromString(const std::string &name) {
  //FIXME slugify the given string first, then strip all dashes (e.g. to
  // allow forest-green or "Forest Green" or forest_green)
  std::string cname = strings::Replace(strings::Lower(name), "-", "");
//  std::replace(cname.begin(), cname.end(), '-', 'x');

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
  } else if (cname.compare("salmon") == 0) {
    return NamedColor::Salmon;
  } else if (cname.compare("silver") == 0) {
    return NamedColor::Silver;
  } else if (cname.compare("tealgreen") == 0) {
    return NamedColor::TealGreen;
  } else if (cname.compare("turquoise") == 0) {
    return NamedColor::Turquoise;
  } else if (cname.compare("rosered") == 0) {
    return NamedColor::RoseRed;
  } else if (cname.compare("invalid") == 0) {
    return NamedColor::Invalid;
  }

  std::stringstream s;
  s << "Could not look up color name corresponding to \"" << name << "\".";
  throw std::invalid_argument(s.str());
}


std::string NamedColorToString(NamedColor color) {
  std::stringstream s;
  switch (color) {
    case NamedColor::Black: return "black";
    case NamedColor::White: return "white";
    case NamedColor::Gray: return "gray";
    case NamedColor::Red: return "red";
    case NamedColor::Green: return "green";
    case NamedColor::Blue: return "blue";
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
    case NamedColor::Salmon: return "salmon";
    case NamedColor::Silver: return "silver";
    case NamedColor::TealGreen: return "teal-green";
    case NamedColor::Turquoise: return "turquoise";
    case NamedColor::RoseRed: return "rose-red";

    case NamedColor::Invalid: return "invalid";

    default:
      s << "No string representation available for this named color ("
        << static_cast<unsigned short>(color) << ")";
      throw std::runtime_error(s.str());
  }
}


Color::Color(const NamedColor color, double alpha) {
  this->alpha = alpha;
  std::stringstream s;

  switch(color) {
    case NamedColor::Black:
      red = green = blue = 0.0; break;

    case NamedColor::White:
      red = green = blue = 1.0; break;

    case NamedColor::Gray:  // "Grey" is simply an alias
      red = green = blue = 0.5; break;

    case NamedColor::Red:
      red = 1.0; green = blue = 0.0; break;

    case NamedColor::Green:
      green = 1.0; red = blue = 0.0; break;

    case NamedColor::Blue:
      blue = 1.0; red = green = 0.0; break;

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

//TODO continue!
    case NamedColor::Purple:
      red = 0.63; green = 0.13; blue = 0.94; break;
//TODO continue!

    default:
      s << "No color code available for named color \""
        << NamedColorToString(color) << "\".";
      throw std::runtime_error(s.str());
  }
}


Color Color::Inverse() const {
  return Color(1.0 - red, 1.0 - green, 1.0 - blue, alpha);
}


bool Color::IsValid() const
{
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


std::string Color::ToString() const
{
  std::stringstream s;
  s << "rgba(" << std::fixed << std::setprecision(2)
    << red << ", " << green << ", " << blue << ", "
    << alpha << ")";
  return s.str();
}

std::tuple<unsigned char, unsigned char, unsigned char, double>
Color::ToRGBA() const
{
  return std::make_tuple(static_cast<unsigned char>(red * 255),
        static_cast<unsigned char>(green * 255),
        static_cast<unsigned char>(blue * 255),
        alpha);
}


std::string Color::ToHexString() const
{
  std::string webcode("#000000");
  // RGB is easier to work with
  auto RGBa = ToRGBA();

  // Mapping from [0,15] to corresponding hex code character
  std::map<unsigned char, char> hex2char {
    {0, '0'}, {1, '1'}, {2, '2'}, {3, '3'}, {4, '4'},
    {5, '5'}, {6, '6'}, {7, '7'}, {8, '8'}, {9, '9'},
    {10, 'A'}, {11, 'B'}, {12, 'C'}, {13, 'D'},
    {14, 'E'}, {15, 'F'}
  };

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

  return webcode;
}


bool operator==(const Color& lhs, const Color& rhs)
{
  return eps_equal(lhs.red, rhs.red) && eps_equal(lhs.green, rhs.green)
      && eps_equal(lhs.blue, rhs.blue) && eps_equal(lhs.alpha, rhs.alpha);
}


bool operator!=(const Color& lhs, const Color& rhs)
{
  return !(lhs == rhs);
}

Color rgba(double r, double g, double b, double alpha)
{
  return Color(cast_01(r), cast_01(g), cast_01(b),
               cast_01(alpha));
}

Color RGBA(double R, double G, double B, double alpha)
{
  return Color(cast_RGB(R/255.0), cast_RGB(G/255.0), cast_RGB(B/255.0),
               alpha);
}

Color InvalidColor()
{
  Color c;
  c.red = c.green = c.blue = c.alpha = -1.0;
  return c;
}

//TODO maybe add more named colors?
// Similar to: https://matplotlib.org/3.5.0/_images/sphx_glr_named_colors_003.png
// Check mixing ratios at: https://www.canva.com/colors/color-meanings/
//namespace colors
//{

//Color Magenta(double alpha)
//{
//  return rgba(1, 0, 1, alpha);
//}

//Color Turquoise(double alpha)
//{
//  return rgba(0.19, 0.84, 0.78, alpha);
//}

//Color Orange(double alpha)
//{
//  return rgba(1, 0.65, 0, alpha);
//}

//Color Orchid(double alpha)
//{
//  return rgba(0.86, 0.44, 0.84, alpha);
//}

//Color Maroon(double alpha)
//{
//  return rgba(0.5, 0, 0, alpha);
//}

//Color Silver(double alpha)
//{
//  return rgba(0.75, 0.75, 0.75, alpha);
//}

//Color Gold(double alpha)
//{
//  return rgba(1, 0.84, 0, alpha);
//}



//Color TealGreen(double alpha)
//{
//  return rgba(0, 0.43, 0.36, alpha);
//}

//Color LimeGreen(double alpha)
//{
//  return rgba(0.2, 0.8, 0.2, alpha);
//}

//Color NavyBlue(double alpha)
//{
//  return rgba(0, 0, 0.5, alpha);
//}

//Color Indigo(double alpha)
//{
//  return rgba(0.3, 0, 0.51, alpha);
//}


//Color MidnightBlue(double alpha)
//{
//  return rgba(0.1, 0.1, 0.44, alpha);
//}

//Color Salmon(double alpha)
//{
//  return rgba(0.98, 0.5, 0.45, alpha);
//}

//Color RoseRed(double alpha)
//{
//  return rgba(1, 0.01, 0.24, alpha);
//}

//Color Olive(double alpha)
//{
//  return rgba(0.5, 0.5, 0, alpha);
//}

//Color LightBlue(double alpha)
//{
//  return rgba(0.68, 0.85, 0.9, alpha);
//}

//Color Lavender(double alpha)
//{
//  return rgba(0.9, 0.9, 0.98, alpha);
//}

//Color Ivory(double alpha)
//{
//  return rgba(1, 1, 0.94, alpha);
//}
//} // namespace colors

} // namespace viren2d
