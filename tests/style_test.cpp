#include <exception>

#include <gtest/gtest.h>

#include <viren2d/styles.h>

TEST(StyleTest, LineStyle) {
  // Default initialization should yield a valid/sane style
  auto style = viren2d::LineStyle();
  EXPECT_TRUE(style.IsValid());

  style.line_width = 0;
  EXPECT_FALSE(style.IsValid());

  style.line_width = 0.4;
  EXPECT_TRUE(style.IsValid());

  style.color = viren2d::Color();
  EXPECT_FALSE(style.color.IsValid());
  EXPECT_FALSE(style.IsValid());
}
