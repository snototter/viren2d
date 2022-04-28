#include <type_traits>
#include <exception>

#include <gtest/gtest.h>

#include <viren2d/colors.hpp>
#include <viren2d/math.hpp>

::testing::AssertionResult CheckColor(const viren2d::Color &color,
                                      double red, double green, double blue, double alpha) {
  if (viren2d::eps_equal(color.red, red)
      && viren2d::eps_equal(color.green, green)
      && viren2d::eps_equal(color.blue, blue)
      && viren2d::eps_equal(color.alpha, alpha))
    return ::testing::AssertionSuccess();
  else
    return ::testing::AssertionFailure()
           << "Color " << color.ToString() << " is not equal to ("
           << red << ", " << green << ", " << blue << ", " << alpha << ")";
}



// Enum value iterator from https://stackoverflow.com/a/31836401
template <typename C, C beginval, C endval>
class ContinuousEnumIterator {
 private:
  typedef typename std::underlying_type<C>::type val_t;
  int val;

 public:
  ContinuousEnumIterator(const C & f)
    : val(static_cast<val_t>(f))
  {}

  ContinuousEnumIterator()
    : val(static_cast<val_t>(beginval))
  {}

  ContinuousEnumIterator operator++() {
    ++val;
    return *this;
  }

  C operator*() {
    return static_cast<C>(val);
  }

  ContinuousEnumIterator begin() const {
    return *this;  // default ctor is good
  }

  ContinuousEnumIterator end() const {
      static const ContinuousEnumIterator enditer=++ContinuousEnumIterator(endval);  // cache it
      return enditer;
  }

  bool operator!=(const ContinuousEnumIterator& i) {
    return val != i.val;
  }
};


TEST(ColorTest, BasicInitialization) {
  auto color = viren2d::Color();
  EXPECT_FALSE(color.IsValid());

  // Init from rgba values
  color = viren2d::Color(0.3, 0.2, 0.5, 0.7);
  EXPECT_TRUE(color.IsValid());
  EXPECT_TRUE(CheckColor(color, 0.3, 0.2, 0.5, 0.7));

  // Copy constructor
  viren2d::Color copy(color);
  EXPECT_EQ(copy, color);
  EXPECT_TRUE(CheckColor(copy, color.red, color.green,
                         color.blue, color.alpha));

  // Saturation casts should work
  color = viren2d::Color(255.0, 1000.0, 73000.0, 200.0);
  EXPECT_TRUE(color.IsValid());
  EXPECT_TRUE(CheckColor(color, 1, 1, 1, 1));

  color = viren2d::Color(-1.0, -0.001, 0.5, 0.99);
  EXPECT_TRUE(color.IsValid());
  EXPECT_TRUE(CheckColor(color, 0.0, 0.0, 0.5, 0.99));
}


TEST(ColorTest, NamedColors) {
  // Ensure that all NamedColors are properly mapped (from & to string)
  typedef ContinuousEnumIterator<viren2d::NamedColor,
              viren2d::NamedColor::Black,
              viren2d::NamedColor::Invalid> NamedColorIterator;

  for (viren2d::NamedColor cn: NamedColorIterator()) {
    auto c_enum = viren2d::Color(cn);
    auto c_name = viren2d::Color(viren2d::NamedColorToString(cn));
    EXPECT_EQ(c_enum, c_name) << "Mismatch between enum & string construction: "
                              << c_enum << " vs " << c_name
                              << " (" << viren2d::NamedColorToString(cn) << ")";
  }

  EXPECT_TRUE(CheckColor(viren2d::Color("RED"), 1.0, 0.0, 0.0, 1.0));
  EXPECT_TRUE(CheckColor(viren2d::Color("CyAn"), 0.0, 1.0, 1.0, 1.0));
  EXPECT_TRUE(CheckColor(viren2d::Color("bLaCk", 0.3), 0.0, 0.0, 0.0, 0.3));

  EXPECT_TRUE(CheckColor(viren2d::Color("inVALid", 0.25), -1.0, -1.0, -1.0, 0.25));
  EXPECT_FALSE(viren2d::Color(viren2d::NamedColor::Invalid).IsValid());

  EXPECT_THROW(viren2d::Color("bLaCk!!", 0.3), std::invalid_argument);

}

//TODO inverse / complementary
// https://en.wikipedia.org/wiki/Complementary_colors
// could test both by computing the expected values AND for some named colors: (red).inverse() == Cyan

//TODO test operator (scale color by factor in [0, 1])
//TODO rgba(), RGBA()
