#include <sstream>
#include <iomanip>
#include <map>

#include <vivi/colors.hpp>
#include <vivi/math.hpp>

namespace vivi
{

double cast_01(double v)
{
  return saturation_cast<double>(v, 0.0, 1.0);
}


double cast_RGB(double v)
{
  return saturation_cast<double>(v, 0.0, 255.0);
}


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

  std::map<int, char> hex {
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
  int div = (int)std::get<0>(RGBa) / 16;
  int rem = (int)std::get<0>(RGBa) % 16;
  webcode[1] = hex[div];
  webcode[2] = hex[rem];

  div = (int)std::get<1>(RGBa) / 16;
  rem = (int)std::get<1>(RGBa) % 16;
  webcode[3] = hex[div];
  webcode[4] = hex[rem];

  div = (int)std::get<2>(RGBa) / 16;
  rem = (int)std::get<2>(RGBa) % 16;
  webcode[5] = hex[div];
  webcode[6] = hex[rem];

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

} // namespace vivi
