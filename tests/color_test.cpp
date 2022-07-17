#include <exception>

#include <gtest/gtest.h>

#include <werkzeugkiste/strings/strings.h>
#include <werkzeugkiste/geometry/utils.h>

#include <viren2d/colors.h>
#include <helpers/enum.h>

namespace wgu = werkzeugkiste::geometry;

::testing::AssertionResult CheckColor(const viren2d::Color &color,
                                      double red, double green,
                                      double blue, double alpha) {
  if (wgu::eps_equal(color.red, red)
      && wgu::eps_equal(color.green, green)
      && wgu::eps_equal(color.blue, blue)
      && (std::fabs(color.alpha - alpha) < 0.001)) {
    return ::testing::AssertionSuccess();
  } else {
    return ::testing::AssertionFailure()
           << "Color " << color.ToString() << " is not equal to ("
           << red << ", " << green << ", " << blue << ", " << alpha << ")";
  }
}


TEST(ColorTest, BasicInitialization) {
  // Default initialization should yield an invalid color
  auto color = viren2d::Color();
  EXPECT_FALSE(color.IsValid());

  EXPECT_FALSE(viren2d::Color::Invalid.IsValid());
  EXPECT_TRUE(color == viren2d::Color::Invalid);

  // Init from rgba values
  color = viren2d::Color(0.3, 0.2, 0.5, 0.7);
  EXPECT_TRUE(color.IsValid());
  EXPECT_TRUE(CheckColor(color, 0.3, 0.2, 0.5, 0.7));

  auto c2 = viren2d::rgba(color.red, color.green,
                          color.blue, color.alpha);
  EXPECT_EQ(c2, color);

  // Init from RGB (range [0, 255])
  c2 = viren2d::RGBa(color.red * 255.0, color.green * 255.0,
                     color.blue * 255.0, color.alpha);
  EXPECT_EQ(c2, color);

  // Init from static members
  color = viren2d::Color::Magenta.WithAlpha(0.25);
  EXPECT_EQ(color, viren2d::Color("magenta!25"));

  color = viren2d::Color::Blue.WithAlpha(0.9);
  EXPECT_EQ(color, viren2d::Color("blue!90"));

  // Copy constructor
  viren2d::Color copy(color);
  EXPECT_EQ(copy, color);
  EXPECT_TRUE(CheckColor(copy, color.red, color.green,
                         color.blue, color.alpha));

  // Saturation casts should work for the standard c'tor:
  color = viren2d::Color(255.0, 1000.0, 73000.0, 200.0);
  EXPECT_TRUE(color.IsValid());
  EXPECT_TRUE(CheckColor(color, 1.0, 1.0, 1.0, 1.0));

  color = viren2d::Color(-1.0, -0.001, 0.5, 0.99);
  EXPECT_TRUE(color.IsValid());
  EXPECT_TRUE(CheckColor(color, 0.0, 0.0, 0.5, 0.99));

  // ... but also for the RGBa convenience function
  color = viren2d::RGBa(300.0, -1.0, 1234.0, -3.0);
  EXPECT_TRUE(color.IsValid());
  EXPECT_TRUE(CheckColor(color, 1.0, 0.0, 1.0, 0.0));

  color = viren2d::RGBa(-3.0, 1000.0, -0.0001, 0.3);
  EXPECT_TRUE(color.IsValid());
  EXPECT_TRUE(CheckColor(color, 0.0, 1.0, 0.0, 0.3));

  // Copy constructions
  auto copy1(color);
  EXPECT_EQ(color, copy1);

  auto copy2(viren2d::Color::Invalid);
  EXPECT_EQ(copy2, viren2d::Color::Invalid);

  color = viren2d::RGBa(200, 0, 100, 0.6);
  auto copy3(color);
  EXPECT_EQ(copy3, color);
}


TEST(ColorTest, StringConversion) {
  viren2d::Color color = "cyan!20";
  EXPECT_EQ(color, viren2d::Color::Cyan.WithAlpha(0.20));

  color = viren2d::Color(std::string("taupe!80"));
  EXPECT_EQ(color, viren2d::Color(viren2d::NamedColor::Taupe, 0.8));

  color = viren2d::Color("navy----blue!77");
  EXPECT_EQ(color, viren2d::Color(viren2d::NamedColor::NavyBlue, 0.77));
}


TEST(ColorTest, ValidityChecks) {
  // Is color valid?
  viren2d::Color color;
  EXPECT_FALSE(color.IsValid());

  color = viren2d::Color("black");
  EXPECT_TRUE(color.IsValid());

  color = viren2d::Color("invalid");
  EXPECT_FALSE(color.IsValid());

  color = viren2d::Color("forest-green");
  EXPECT_TRUE(color.IsValid());

  color = viren2d::Color(viren2d::NamedColor::Invalid);
  EXPECT_FALSE(color.IsValid());

  color = viren2d::Color("navy-blue");
  EXPECT_TRUE(color.IsValid());

  color.red = -1.0;
  EXPECT_FALSE(color.IsValid());

  color.red = 0.0;
  EXPECT_TRUE(color.IsValid());
  color.green = 2.0;
  EXPECT_FALSE(color.IsValid());

  color.green = 0.0;
  EXPECT_TRUE(color.IsValid());
  color.blue = -1.0;
  EXPECT_FALSE(color.IsValid());

  color.blue = 0.0;
  EXPECT_TRUE(color.IsValid());
  color.alpha = -1.0;
  EXPECT_FALSE(color.IsValid());
}

TEST(ColorTest, ShadeOfGrayChecks) {
  // Is color a shade of gray?
  auto color = viren2d::Color(viren2d::NamedColor::Black);
  EXPECT_TRUE(color.IsValid());

  EXPECT_TRUE(color.IsShadeOfGray());
  color.red = 0.02;
  EXPECT_TRUE(color.IsShadeOfGray());
  color.red = 0.03;
  EXPECT_FALSE(color.IsShadeOfGray());
  EXPECT_TRUE(color.IsShadeOfGray(0.05));

  color.red = color.green = color.blue = 0.77;
  EXPECT_TRUE(color.IsShadeOfGray());
  color.green = 0.9;
  EXPECT_FALSE(color.IsShadeOfGray());
}


TEST(ColorTest, AdvancedInitialization) {
  auto color = viren2d::Color("black", 0.75);
  EXPECT_EQ(color, viren2d::Color(viren2d::NamedColor::Black, 0.75));

  // Alpha in the string specification overwrites the parameter
  color = viren2d::Color("black!50", 0.75);
  EXPECT_EQ(color, viren2d::Color(viren2d::NamedColor::Black, 0.5));

  // Alpha must be an integer
  EXPECT_THROW(viren2d::Color("black!!", 0.3), std::invalid_argument);
  EXPECT_THROW(viren2d::Color("white!!70", 0.3), std::invalid_argument);
  EXPECT_THROW(viren2d::Color(std::string("red!0.5"), 0.3), std::invalid_argument);
  EXPECT_THROW(viren2d::Color("red!12.3", 0.3), std::invalid_argument);
  EXPECT_NO_THROW(viren2d::Color(std::string("taupe!30"), 0.3));

  // Alpha must be in [0, 100]
  EXPECT_THROW(viren2d::Color("black!-3", 0.3), std::invalid_argument);
  EXPECT_THROW(viren2d::Color("white!101", 0.3), std::invalid_argument);
  EXPECT_THROW(viren2d::Color("red!999999", 0.3), std::invalid_argument);
  EXPECT_NO_THROW(viren2d::Color("taupe!99"));
  EXPECT_NO_THROW(viren2d::Color("tangerine!0"));

  // Although the following are valid integers in [0,100], we
  // do not support +/- notation.
  EXPECT_THROW(viren2d::Color("tangerine!-0"), std::invalid_argument);
  EXPECT_THROW(viren2d::Color("blue!+30"), std::invalid_argument);

  // Special string initialization: inverse of a color
  color = viren2d::Color("!blue");
  EXPECT_EQ(color, viren2d::Color(viren2d::NamedColor::Yellow));

  EXPECT_THROW(viren2d::Color("!!blue"), std::invalid_argument);

  color = viren2d::Color("!black", 0.5);
  EXPECT_EQ(color, viren2d::Color(viren2d::NamedColor::White, 0.5));

  color = viren2d::Color("!green!40", 0.9);
  EXPECT_EQ(color, viren2d::Color(viren2d::NamedColor::Magenta, 0.4));

  color = viren2d::Color("-red!60", 0.9);
  EXPECT_EQ(color, viren2d::Color(viren2d::NamedColor::Cyan, 0.6));

  color = viren2d::Color("-navy-blue!60", 0.9);
  EXPECT_EQ(color, viren2d::Color("!navy-blue!60"));
  EXPECT_EQ(color, viren2d::RGBa(255, 255, 127.5, 0.6));
}


TEST(ColorTest, Webcodes) {
  // Test initialization via webcodes/hex codes
  auto color = viren2d::ColorFromHexString("#000000", 0.1);
  EXPECT_EQ(color, viren2d::Color(viren2d::NamedColor::Black, 0.1));
  EXPECT_EQ(color.ToHexString(), "#00000019");

  color = viren2d::ColorFromHexString("#fFfFfF", 0.3);
  EXPECT_EQ(color, viren2d::Color(viren2d::NamedColor::White, 0.3));
  EXPECT_EQ(color.ToHexString(), "#ffffff4c");

  // Invalid inputs
  EXPECT_THROW(viren2d::ColorFromHexString("abcd"), std::invalid_argument);
  EXPECT_THROW(viren2d::ColorFromHexString("#abc"), std::invalid_argument);
  EXPECT_THROW(viren2d::ColorFromHexString("#abcdefghf"), std::invalid_argument);

  // Sweep over each hex digit separately:
  for (size_t idx = 0; idx < 8; ++idx) {
    std::string webcode("#000000FF");
    for (unsigned char digit = 0; digit < 16; ++digit) {
      webcode[idx+1] = (digit < 10) ? ('0' + digit)
                                  : ('A' + digit - 10);
      color = viren2d::Color(webcode);
      EXPECT_EQ(werkzeugkiste::strings::Upper(color.ToHexString()),
                webcode);
    }
  }

  // Hardcoded values:
  color = viren2d::ColorFromHexString("#0f5A12");
  EXPECT_EQ(color, viren2d::RGBa(15, 90, 18));

  color = viren2d::ColorFromHexString("#5500ba");
  EXPECT_EQ(color, viren2d::RGBa(85, 0, 186));

  color = viren2d::ColorFromHexString("#ea8435ff");
  EXPECT_EQ(color, viren2d::RGBa(234, 132, 53));

  color = viren2d::ColorFromHexString("#ea843500");
  EXPECT_EQ(color, viren2d::RGBa(234, 132, 53, 0));

  color = viren2d::ColorFromHexString("#ea843534");
  EXPECT_TRUE(CheckColor(color, 234.0/255, 132.0/255, 53.0/255, 0.2039));

  color = viren2d::Color();
  EXPECT_FALSE(color.IsValid());
  EXPECT_EQ(color.ToHexString(), "#????????");
}


TEST(ColorTest, NamedColors) {
  // Ensure that all NamedColors are properly mapped (bothm from & to string)
  typedef viren2d::ContinuousEnumIterator<viren2d::NamedColor,
              viren2d::NamedColor::Black,
              viren2d::NamedColor::Invalid> NamedColorIterator;

  for (viren2d::NamedColor cn: NamedColorIterator()) {
    auto c_enum = viren2d::Color(cn);
    auto c_name = viren2d::Color(viren2d::NamedColorToString(cn));
    EXPECT_EQ(c_enum, c_name) << "Mismatch between enum & string construction: "
                              << c_enum << " vs " << c_name
                              << " (" << viren2d::NamedColorToString(cn) << ")";
  }

  // Case should be ignored:
  EXPECT_TRUE(CheckColor(viren2d::Color("RED"), 1.0, 0.0, 0.0, 1.0));
  EXPECT_TRUE(CheckColor(viren2d::Color("CyAn"), 0.0, 1.0, 1.0, 1.0));
  EXPECT_TRUE(CheckColor(viren2d::Color("bLaCk", 0.3), 0.0, 0.0, 0.0, 0.3));

  // We can also instantiate an invalid color via enum/string:
  EXPECT_TRUE(CheckColor(viren2d::Color("inVALid", 0.25), -1.0, -1.0, -1.0, -1.0));
  EXPECT_FALSE(viren2d::Color(viren2d::NamedColor::Invalid).IsValid());

  // String representations ignore white space, hyphen and underscores:
  EXPECT_TRUE(viren2d::Color("  wh i t-e ") == viren2d::Color::White);
  EXPECT_TRUE(viren2d::Color("black\t\n") == viren2d::Color::Black);
  EXPECT_TRUE(viren2d::Color("navy_blue") == viren2d::Color("navy-blue"));
  EXPECT_TRUE(viren2d::Color("Forest Green!40") == viren2d::Color("forestgreen", 0.4));
}


TEST(ColorTest, ComplementaryColors) {
  auto color = viren2d::Color(viren2d::NamedColor::Black);
  EXPECT_EQ(color.Inverse(), viren2d::Color::White);

  EXPECT_EQ(viren2d::Color::White.Inverse(),
            viren2d::Color::Black);

  EXPECT_EQ(viren2d::Color::Red.Inverse(),
            viren2d::Color::Cyan);

  EXPECT_EQ(viren2d::Color::Green.Inverse(),
            viren2d::Color::Magenta);

  EXPECT_EQ(viren2d::Color::Blue.Inverse(),
            viren2d::Color::Yellow);

  // Special handling for shades of gray
  for (int v = 0; v < 128; ++v) {
    color = viren2d::RGBa(v, v, v).Inverse();
    EXPECT_EQ(color, viren2d::Color::White);
  }

  for (int v = 128; v < 256; ++v) {
    color = viren2d::RGBa(v, v, v).Inverse();
    EXPECT_EQ(color, viren2d::Color::Black);
  }
}


TEST(ColorTest, Operators) {
  // Equality
  EXPECT_TRUE(viren2d::Color::Black != viren2d::Color::Red);
  EXPECT_TRUE(viren2d::Color::Black == viren2d::Color::Black);
  EXPECT_TRUE(viren2d::Color::Cyan == viren2d::Color(viren2d::NamedColor::Cyan));
  EXPECT_TRUE(viren2d::Color::Cyan != viren2d::Color("midnight-blue"));
  EXPECT_TRUE(viren2d::Color::Magenta == viren2d::Color("magenta"));
  EXPECT_TRUE(viren2d::Color::Yellow == viren2d::Color("#ffff00"));

  EXPECT_TRUE(viren2d::Color::Invalid == viren2d::Color("invalid"));
  EXPECT_TRUE(viren2d::Color::Invalid == viren2d::Color("INVALID"));
  EXPECT_TRUE(viren2d::Color::Invalid == viren2d::Color("none"));
  EXPECT_TRUE(viren2d::Color::Invalid == viren2d::Color("None"));

  EXPECT_TRUE(viren2d::Color::Black != viren2d::Color("black!60"));
  EXPECT_TRUE(viren2d::Color::Red != viren2d::Color("red!90"));
  EXPECT_TRUE(viren2d::Color(viren2d::NamedColor::Red, 0.9) == viren2d::Color("red!90"));

  // Scalar multiplication (alpha should not be changed)
  auto color = 0.5 * viren2d::Color::Cyan;
  EXPECT_TRUE(CheckColor(color, 0.0, 0.5, 0.5, 0.5));
  // Values should be clamped
  color.alpha = 0.7;
  color *= 3;
  EXPECT_TRUE(CheckColor(color, 0.0, 1.0, 1.0, 1.0));

  color = viren2d::Color::White * 0.5;
  EXPECT_TRUE(CheckColor(color, 0.5, 0.5, 0.5, 0.5));
  color.alpha = 0.7;
  auto copy(color);
  color *= 1.5;
  EXPECT_TRUE(CheckColor(color, 0.75, 0.75, 0.75, 1.0));
  EXPECT_EQ(copy * 1.5, color);


  color = viren2d::Color::Magenta / 2.0;
  EXPECT_TRUE(CheckColor(color, 0.5, 0.0, 0.5, 0.5));
  color.alpha = 1.0;
  copy = viren2d::Color(color);
  color /= 5;
  EXPECT_TRUE(CheckColor(color, 0.1, 0.0, 0.1, 0.2));
  EXPECT_EQ((viren2d::Color::Magenta / 10.0).WithAlpha(0.2), color);

  // Addition with saturation casts
  auto add = color + color;
  EXPECT_EQ(2 * color, add);
  copy.green = 0.3;
  copy.alpha = 0.1;
  add += copy;
  EXPECT_EQ(add, viren2d::rgba(0.7, 0.3, 0.7, 0.5));
  // but the operand should not have changed
  EXPECT_EQ((viren2d::Color::Magenta / 10.0).WithAlpha(0.2), color);
  // saturation
  add += viren2d::Color::White;
  EXPECT_EQ(add, "white");

  // Subtract
  add -= 2 * viren2d::Color::Magenta;
  EXPECT_EQ(add, "GREEN!0");

  add = copy - color;
  EXPECT_EQ(add, viren2d::rgba(0.4, 0.3, 0.4, 0.0));
}
