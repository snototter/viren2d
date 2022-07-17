#include <exception>

#include <gtest/gtest.h>

#include <viren2d/styles.h>
#include <viren2d/drawing.h>

#include <helpers/enum.h>


TEST(StyleTest, LineStyleBasics) {
  // Default initialization should yield a valid/sane style
  auto style = viren2d::LineStyle();
  EXPECT_TRUE(style.IsValid());
  EXPECT_FALSE(style.IsDashed());

  style.width = 0;
  EXPECT_FALSE(style.IsValid());

  style.width = 0.4;
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


TEST(StyleTest, ArrowStyleBasics) {
  // Default initialization should yield a valid/sane style
  auto style = viren2d::ArrowStyle();
  EXPECT_TRUE(style.IsValid());

  // Mess around with the base line style
  style.width = 0;
  EXPECT_FALSE(style.IsValid());
  style.width = 0.4;
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


TEST(StyleTest, ArrowTipLength) {
  auto style = viren2d::ArrowStyle();
  style.tip_length = 0.1;

  EXPECT_DOUBLE_EQ(style.TipLengthForShaft(200.0), 20.0);
  EXPECT_DOUBLE_EQ(style.TipLengthForShaft(viren2d::Vec2d(10, 10), viren2d::Vec2d(10, 30)), 2.0);

  style.tip_length = 90;
  EXPECT_DOUBLE_EQ(style.TipLengthForShaft(200.0), 90.0);
  EXPECT_DOUBLE_EQ(style.TipLengthForShaft(30.0), 90.0);

  // Default line join should be miter
  EXPECT_EQ(style.join, viren2d::LineJoin::Miter);
  style.width = 5;
  style.tip_angle = 5;
  EXPECT_DOUBLE_EQ(style.TipOffset(), style.width / 2.0);
  style.tip_angle = 6;
  EXPECT_DOUBLE_EQ(style.TipOffset(), 23.916930583764067);
  style.tip_angle = 45;
  EXPECT_DOUBLE_EQ(style.TipOffset(), 3.5355339059327378);

  style.join = viren2d::LineJoin::Round;
  style.tip_angle = 5;
  EXPECT_DOUBLE_EQ(style.TipOffset(), style.width / 2.0);
  style.tip_angle = 6;
  EXPECT_DOUBLE_EQ(style.TipOffset(), style.width / 2.0);

  style.join = viren2d::LineJoin::Bevel;
  EXPECT_DOUBLE_EQ(style.TipOffset(), style.width / 2.0);
  style.tip_angle = 6;
  EXPECT_DOUBLE_EQ(style.TipOffset(), style.width / 2.0);
}


TEST(StyleTest, Offsets) {
  // Default initialization should yield a valid/sane style
  auto style = viren2d::LineStyle();
  EXPECT_TRUE(style.IsValid());

  // Default line cap should be butt:
  EXPECT_EQ(style.cap, viren2d::LineCap::Butt);
  EXPECT_DOUBLE_EQ(style.CapOffset(), 0.0);

  style.cap = viren2d::LineCap::Round;
  EXPECT_DOUBLE_EQ(style.CapOffset(), style.width / 2.0);

  // Default join should be miter
  EXPECT_EQ(style.join, viren2d::LineJoin::Miter);
  style.width = 2.0;
  EXPECT_DOUBLE_EQ(style.JoinOffset(10), style.width / 2.0);
  // By default, cairo switches to bevel joins at 10 deg
  // interior angle
  EXPECT_DOUBLE_EQ(style.JoinOffset(5), style.width / 2.0);
  EXPECT_DOUBLE_EQ(style.JoinOffset(12), 9.5667722335056276);
  EXPECT_DOUBLE_EQ(style.JoinOffset(45), 2.6131259297527532);

  style.join = viren2d::LineJoin::Bevel;
  EXPECT_DOUBLE_EQ(style.JoinOffset(10), style.width / 2.0);
  EXPECT_DOUBLE_EQ(style.JoinOffset(12), style.width / 2.0);
  EXPECT_DOUBLE_EQ(style.JoinOffset(45), style.width / 2.0);

  style.join = viren2d::LineJoin::Round;
  EXPECT_DOUBLE_EQ(style.JoinOffset(10), style.width / 2.0);
  EXPECT_DOUBLE_EQ(style.JoinOffset(12), style.width / 2.0);
  EXPECT_DOUBLE_EQ(style.JoinOffset(45), style.width / 2.0);
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

//TODO test textstyle
//TODO test boundingbox2dstyle

TEST(StyleTest, MarkerStyleTest) {
  // Compare 2 MarkerStyle objects
  auto style1 = viren2d::MarkerStyle();
  auto style2 = viren2d::MarkerStyle();
  EXPECT_EQ(style1, style2);

  style1.color = "azure!99";
  EXPECT_NE(style1, style2);

  EXPECT_TRUE(style1.IsValid());
  EXPECT_TRUE(style2.IsValid());

  style1.size = -1;
  EXPECT_FALSE(style1.IsValid());

  style1.marker = viren2d::Marker::Heptagram;
  EXPECT_FALSE(style1.IsValid());
  style1.size = 10;
  style1.filled = false;
  style1.thickness = -1;
  EXPECT_FALSE(style1.IsValid());
  style1.filled = true;
  EXPECT_TRUE(style1.IsValid());
  style1.filled = false;
  style1.thickness = 1;
  EXPECT_TRUE(style1.IsValid());

  style1 = style2;
  EXPECT_EQ(style1, style2);
  style1.marker = viren2d::MarkerFromChar('5');
  style2.marker = viren2d::Marker::Pentagon;
  EXPECT_NE(style1, style2);
  style2.marker = viren2d::Marker::Pentagram;
  EXPECT_EQ(style1, style2);


  // Ensure that the char to marker mapping is correctly
  // implemented for all markers
  // Ensure that all NamedColors are properly mapped (bothm from & to string)
  typedef viren2d::ContinuousEnumIterator<viren2d::Marker,
              viren2d::Marker::Point,
              viren2d::Marker::Enneagon> MarkerIterator;
  style1 = style2;
  EXPECT_EQ(style1, style2);
  for (viren2d::Marker marker: MarkerIterator()) {
    style1.marker = marker;
    char m_code = viren2d::MarkerToChar(marker);
    style2.marker = viren2d::MarkerFromChar(m_code);
    EXPECT_EQ(style1, style2) << "Mismatch between marker enum & char construction: "
                              << style1 << " vs " << style2;
  }
}
