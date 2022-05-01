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


TEST(StyleTest, ArrowStyle) {
  // Default initialization should yield a valid/sane style
  auto style = viren2d::ArrowStyle();
  EXPECT_TRUE(style.IsValid());

  // Mess around with the base line style
  style.line_width = 0;
  EXPECT_FALSE(style.IsValid());
  style.line_width = 0.4;
  EXPECT_TRUE(style.IsValid());

  style.color = viren2d::Color();
  EXPECT_FALSE(style.color.IsValid());
  EXPECT_FALSE(style.IsValid());
  style.color = viren2d::Color(viren2d::NamedColor::Carrot);
  EXPECT_TRUE(style.IsValid());

  // Mess around with arrow-specific style
  style.tip_angle = 0;
  EXPECT_FALSE(style.IsValid());
  style.tip_angle = 15;
  EXPECT_TRUE(style.IsValid());

  style.tip_length = 0;
  EXPECT_FALSE(style.IsValid());
  style.tip_length = 3;
  EXPECT_TRUE(style.IsValid());
}


TEST(StyleTest, LineOperators) {
  // Compare 2 LineStyle objects
  auto line_style1 = viren2d::LineStyle();
  auto line_style2 = viren2d::LineStyle();
  EXPECT_EQ(line_style1, line_style2);

  line_style2.dash_pattern = {10, 20};
  EXPECT_NE(line_style1, line_style2);

  // Compare 2 ArrowStyle objects
  auto arrow_style1 = viren2d::ArrowStyle();
  auto arrow_style2 = viren2d::ArrowStyle();
  EXPECT_EQ(arrow_style1, arrow_style2);

  arrow_style1.tip_length = 30;
  EXPECT_NE(arrow_style1, arrow_style2);
  arrow_style1.tip_length = arrow_style2.tip_length;
  EXPECT_EQ(arrow_style1, arrow_style2);

  arrow_style2.tip_angle = 99;
  EXPECT_NE(arrow_style1, arrow_style2);
  arrow_style2.tip_angle = arrow_style1.tip_angle;
  EXPECT_EQ(arrow_style1, arrow_style2);

  // ArrowStyle should differ if we change its
  // base LineStyle properties:
  arrow_style2.dash_pattern = {10, 20};
  EXPECT_NE(arrow_style1, arrow_style2);
  arrow_style2.dash_pattern = {};
  EXPECT_EQ(arrow_style1, arrow_style2);
}

