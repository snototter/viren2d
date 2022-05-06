#include <exception>

#include <gtest/gtest.h>

#include <viren2d/styles.h>


TEST(StyleTest, LineStyleBasics) {
  // Default initialization should yield a valid/sane style
  auto style = viren2d::LineStyle();
  EXPECT_TRUE(style.IsValid());
  EXPECT_FALSE(style.IsDashed());

  style.line_width = 0;
  EXPECT_FALSE(style.IsValid());

  style.line_width = 0.4;
  EXPECT_TRUE(style.IsValid());

  style.dash_pattern = {10.0, 20.0};
  EXPECT_TRUE(style.IsValid());
  EXPECT_TRUE(style.IsDashed());

  style.color = viren2d::Color();
  EXPECT_FALSE(style.color.IsValid());
  EXPECT_FALSE(style.IsValid());

  style.color = "black!80";
  EXPECT_TRUE(style.IsValid());
}



TEST(StyleTest, DefaultLineStyle) {
  const auto original_default_style = viren2d::LineStyle();
  // In the second iteration, the default style will be changed
  for (int i = 0; i < 2; ++i) {
    viren2d::LineStyle ls;
    EXPECT_EQ(ls, viren2d::GetDefaultLineStyle());

    if (i > 0) {
      EXPECT_NE(ls, original_default_style);
    }

    // Change the style
    ls = viren2d::LineStyle(3 + i, "forest-green");
    EXPECT_NE(ls, original_default_style);
    EXPECT_NE(ls, viren2d::GetDefaultLineStyle());

    // Make it the new default
    viren2d::SetDefaultLineStyle(ls);
    EXPECT_EQ(ls, viren2d::GetDefaultLineStyle());
  }
  viren2d::SetDefaultLineStyle(original_default_style);
}


TEST(StyleTest, ArrowStyleBasics) {
  // Default initialization should yield a valid/sane style
  auto style = viren2d::ArrowStyle();
  EXPECT_TRUE(style.IsValid());

  // Mess around with the base line style
  style.line_width = 0;
  EXPECT_FALSE(style.IsValid());
  style.line_width = 0.4;
  EXPECT_TRUE(style.IsValid());
  EXPECT_FALSE(style.IsDashed());

  style.color = viren2d::Color();
  EXPECT_FALSE(style.color.IsValid());
  EXPECT_FALSE(style.IsValid());
  style.color = viren2d::Color(viren2d::NamedColor::Carrot);
  EXPECT_TRUE(style.IsValid());

  style.dash_pattern = {5};
  EXPECT_TRUE(style.IsValid());
  EXPECT_TRUE(style.IsDashed());

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


TEST(StyleTest, DefaultArrowStyle) {
  const viren2d::ArrowStyle original_default_style;
  // In the second iteration, the default style will be changed
  for (int i = 0; i < 2; ++i) {
    viren2d::ArrowStyle ls;
    EXPECT_EQ(ls, viren2d::GetDefaultArrowStyle());

    if (i > 0) {
      EXPECT_NE(ls, original_default_style);
    }

    // Change the style
    ls = viren2d::ArrowStyle(3 + i, "lavender", 0.3);
    EXPECT_NE(ls, original_default_style);
    EXPECT_NE(ls, viren2d::GetDefaultArrowStyle());

    // Make it the new default
    viren2d::SetDefaultArrowStyle(ls);
    EXPECT_EQ(ls, viren2d::GetDefaultArrowStyle());
  }
  viren2d::SetDefaultArrowStyle(original_default_style);
}


TEST(StyleTest, ArrowTipLength) {
  auto style = viren2d::ArrowStyle();
  style.tip_length = 0.1;

  EXPECT_DOUBLE_EQ(style.TipLengthForShaft(200.0), 20.0);
  EXPECT_DOUBLE_EQ(style.TipLengthForShaft(viren2d::Vec2d(10, 10), viren2d::Vec2d(10, 30)), 2.0);

  style.tip_length = 90;
  EXPECT_DOUBLE_EQ(style.TipLengthForShaft(200.0), 90.0);
  EXPECT_DOUBLE_EQ(style.TipLengthForShaft(30.0), 90.0);

  // Default line join should be miter
  EXPECT_EQ(style.line_join, viren2d::LineJoin::Miter);
  style.line_width = 5;
  style.tip_angle = 5;
  EXPECT_DOUBLE_EQ(style.TipOffset(), style.line_width / 2.0);
  style.tip_angle = 6;
  EXPECT_DOUBLE_EQ(style.TipOffset(), 23.916930583764067);
  style.tip_angle = 45;
  EXPECT_DOUBLE_EQ(style.TipOffset(), 3.5355339059327378);

  style.line_join = viren2d::LineJoin::Round;
  style.tip_angle = 5;
  EXPECT_DOUBLE_EQ(style.TipOffset(), style.line_width / 2.0);
  style.tip_angle = 6;
  EXPECT_DOUBLE_EQ(style.TipOffset(), style.line_width / 2.0);

  style.line_join = viren2d::LineJoin::Bevel;
  EXPECT_DOUBLE_EQ(style.TipOffset(), style.line_width / 2.0);
  style.tip_angle = 6;
  EXPECT_DOUBLE_EQ(style.TipOffset(), style.line_width / 2.0);
}


TEST(StyleTest, Offsets) {
  // Default initialization should yield a valid/sane style
  auto style = viren2d::LineStyle();
  EXPECT_TRUE(style.IsValid());

  // Default line cap should be butt:
  EXPECT_EQ(style.line_cap, viren2d::LineCap::Butt);
  EXPECT_DOUBLE_EQ(style.CapOffset(), 0.0);

  style.line_cap = viren2d::LineCap::Round;
  EXPECT_DOUBLE_EQ(style.CapOffset(), style.line_width / 2.0);

  // Default join should be miter
  EXPECT_EQ(style.line_join, viren2d::LineJoin::Miter);
  EXPECT_DOUBLE_EQ(style.JoinOffset(10), style.line_width / 2.0);
  EXPECT_DOUBLE_EQ(style.JoinOffset(12), 9.5667722335056276);
  EXPECT_DOUBLE_EQ(style.JoinOffset(45), 2.6131259297527532);

  style.line_join = viren2d::LineJoin::Bevel;
  EXPECT_DOUBLE_EQ(style.JoinOffset(10), style.line_width / 2.0);
  EXPECT_DOUBLE_EQ(style.JoinOffset(12), style.line_width / 2.0);
  EXPECT_DOUBLE_EQ(style.JoinOffset(45), style.line_width / 2.0);

  style.line_join = viren2d::LineJoin::Round;
  EXPECT_DOUBLE_EQ(style.JoinOffset(10), style.line_width / 2.0);
  EXPECT_DOUBLE_EQ(style.JoinOffset(12), style.line_width / 2.0);
  EXPECT_DOUBLE_EQ(style.JoinOffset(45), style.line_width / 2.0);
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

