#include <sstream>
#include <iomanip>
#include <map>

#include <vivi/colors.hpp>
#include <vivi/math.hpp>

namespace vivi
{
/** @brief Color utilities in anonymuous namespace. */
namespace
{

/** @brief Saturation cast to [0, 1]. */
double cast_01(double v)
{
  return saturation_cast<double>(v, 0.0, 1.0);
}


/** @brief Saturation cast to [0, 255]. */
double cast_RGB(double v)
{
  return saturation_cast<double>(v, 0.0, 255.0);
}

} // namespace (anon)



Color Color::Inverse() const
{
  return Color(1.0 - red, 1.0 - green, 1.0 - blue, alpha);
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

//TODO add more named colors
// Similar to: https://matplotlib.org/3.5.0/_images/sphx_glr_named_colors_003.png
// Check mixing ratios at: https://www.canva.com/colors/color-meanings/
namespace colors
{
Color Black(double alpha)
{
  return rgba(0, 0, 0, alpha);
}

Color White(double alpha)
{
  return rgba(1, 1, 1, alpha);
}

Color Crimson(double alpha)
{
  return rgba(0.6, 0, 0, alpha);
}

Color Purple(double alpha)
{
  return rgba(0.63, 0.13, 0.94, alpha);
}

Color Cyan(double alpha)
{
  return rgba(0, 1, 1, alpha);
}

Color Magenta(double alpha)
{
  return rgba(1, 0, 1, alpha);
}

Color Turquoise(double alpha)
{
  return rgba(0.19, 0.84, 0.78, alpha);
}

Color Orange(double alpha)
{
  return rgba(1, 0.65, 0, alpha);
}

Color Orchid(double alpha)
{
  return rgba(0.86, 0.44, 0.84, alpha);
}

Color Maroon(double alpha)
{
  return rgba(0.5, 0, 0, alpha);
}

Color Silver(double alpha)
{
  return rgba(0.75, 0.75, 0.75, alpha);
}

Color Gold(double alpha)
{
  return rgba(1, 0.84, 0, alpha);
}

Color ForestGreen(double alpha)
{
  return rgba(0.13, 0.55, 0.13, alpha);
}

Color TealGreen(double alpha)
{
  return rgba(0, 0.43, 0.36, alpha);
}

Color LimeGreen(double alpha)
{
  return rgba(0.2, 0.8, 0.2, alpha);
}

Color NavyBlue(double alpha)
{
  return rgba(0, 0, 0.5, alpha);
}

Color Indigo(double alpha)
{
  return rgba(0.3, 0, 0.51, alpha);
}

Color Copper(double alpha)
{
  return rgba(0.72, 0.45, 0.2, alpha);
}

Color Freesia(double alpha)
{
  return rgba(0.97, 0.77, 0.14, alpha);
}

Color MidnightBlue(double alpha)
{
  return rgba(0.1, 0.1, 0.44, alpha);
}

Color Salmon(double alpha)
{
  return rgba(0.98, 0.5, 0.45, alpha);
}

Color RoseRed(double alpha)
{
  return rgba(1, 0.01, 0.24, alpha);
}

Color Olive(double alpha)
{
  return rgba(0.5, 0.5, 0, alpha);
}

Color LightBlue(double alpha)
{
  return rgba(0.68, 0.85, 0.9, alpha);
}

Color Lavender(double alpha)
{
  return rgba(0.9, 0.9, 0.98, alpha);
}

Color Ivory(double alpha)
{
  return rgba(1, 1, 0.94, alpha);
}
} // namespace colors

} // namespace vivi
