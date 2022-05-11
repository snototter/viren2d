#include <exception>

#include <gtest/gtest.h>

#include <viren2d/primitives.h>
#include <helpers/math_utils.h>

TEST(PrimitivesTest, Rectangle) {
  auto r = viren2d::Rect();
  EXPECT_FALSE(r.IsValid());

  EXPECT_THROW(viren2d::Rect({1, 2, 3}), std::invalid_argument);
  EXPECT_NO_THROW(viren2d::Rect({1, 2, 3, 4}));
  EXPECT_NO_THROW(viren2d::Rect({1, 2, 3, 4, 5}));
  EXPECT_NO_THROW(viren2d::Rect({1, 2, 3, 4, 5, 6}));
  EXPECT_THROW(viren2d::Rect({1, 2, 3, 4, 5, 6, 7}), std::invalid_argument);

  r = viren2d::Rect({100, 400}, {80, 60}, 70, 3);
  EXPECT_TRUE(r.IsValid());
  EXPECT_DOUBLE_EQ(r.cx, 100);
  EXPECT_DOUBLE_EQ(r.cy, 400);
  EXPECT_DOUBLE_EQ(r.width, 80);
  EXPECT_DOUBLE_EQ(r.height, 60);
  EXPECT_DOUBLE_EQ(r.rotation, 70);
  EXPECT_DOUBLE_EQ(r.radius, 3);

  // Grid-test both valid and invalid inputs:
  for (double cx : {-123.0, 0.0, 12.0, 0.3}) {
    for (double cy : {-3.0, 0.0, 768.0}) {
      for (double width : {-90, 0, 20}) {
        for (double height : {-123, 0, 6, 180}) {
          viren2d::Rect rect(cx, cy, width, height);
          EXPECT_EQ(rect.IsValid(), (width > 0) && (height > 0));

          EXPECT_DOUBLE_EQ(rect.half_width(), width / 2.0);
          EXPECT_DOUBLE_EQ(rect.half_height(), height / 2.0);

          EXPECT_TRUE(viren2d::eps_zero(rect.rotation));
          EXPECT_TRUE(viren2d::eps_zero(rect.radius));

          rect.width = 1;
          rect.height = 2;
          EXPECT_TRUE(rect.IsValid());

          rect += 17;
          EXPECT_DOUBLE_EQ(rect.cx, cx + 17);
          EXPECT_DOUBLE_EQ(rect.cy, cy + 17);

          rect -= 23;
          EXPECT_DOUBLE_EQ(rect.cx, cx - 6);
          EXPECT_DOUBLE_EQ(rect.cy, cy - 6);

          rect += viren2d::Vec2d(7, 9);
          EXPECT_DOUBLE_EQ(rect.cx, cx + 1);
          EXPECT_DOUBLE_EQ(rect.cy, cy + 3);

          rect -= viren2d::Vec2d(3, 4);
          EXPECT_DOUBLE_EQ(rect.cx, cx - 2);
          EXPECT_DOUBLE_EQ(rect.cy, cy - 1);

          // Try some corner radii
          rect.radius = std::min(rect.width, rect.height) / 2.0;
          EXPECT_TRUE(rect.IsValid());

          rect.radius = std::min(rect.width, rect.height) * 0.6;
          EXPECT_FALSE(rect.IsValid());

          rect.radius = 0.6;
          EXPECT_FALSE(rect.IsValid());

          rect.radius = 0.9;
          EXPECT_FALSE(rect.IsValid());

          rect.radius = 0.3;
          EXPECT_TRUE(rect.IsValid());

          // Try some rotations
          rect.rotation = -10;
          EXPECT_TRUE(rect.IsValid());

          rect.rotation = -360;
          EXPECT_TRUE(rect.IsValid());

          rect.rotation = 170;
          EXPECT_TRUE(rect.IsValid());

          rect.rotation = 400;
          EXPECT_TRUE(rect.IsValid());
        }
      }
    }
  }
}


TEST(PrimitivesTest, Ellipse) {
  auto e = viren2d::Ellipse();
  EXPECT_FALSE(e.IsValid());

  EXPECT_THROW(viren2d::Ellipse({1, 2, 3}), std::invalid_argument);
  EXPECT_NO_THROW(viren2d::Ellipse({1, 2, 3, 4}));
  EXPECT_NO_THROW(viren2d::Ellipse({1, 2, 3, 4, 5}));
  EXPECT_NO_THROW(viren2d::Ellipse({1, 2, 3, 4, 5, 6}));
  EXPECT_NO_THROW(viren2d::Ellipse({1, 2, 3, 4, 5, 6, 7}));
  EXPECT_THROW(viren2d::Ellipse({1, 2, 3, 4, 5, 6, 7, 8}), std::invalid_argument);

  e = viren2d::Ellipse({100, 400}, {80, 60}, 70, 3);
  EXPECT_TRUE(e.IsValid());
  EXPECT_DOUBLE_EQ(e.cx, 100);
  EXPECT_DOUBLE_EQ(e.cy, 400);
  EXPECT_DOUBLE_EQ(e.major_axis, 80);
  EXPECT_DOUBLE_EQ(e.minor_axis, 60);
  EXPECT_DOUBLE_EQ(e.rotation, 70);
  EXPECT_DOUBLE_EQ(e.angle_from, 3);

  // Mess up major/minor (should be automatically adjusted)
  e = viren2d::Ellipse({100, 400}, {30, 60}, 70, 3, 200, false);
  EXPECT_TRUE(e.IsValid());
  EXPECT_DOUBLE_EQ(e.cx, 100);
  EXPECT_DOUBLE_EQ(e.cy, 400);
  EXPECT_DOUBLE_EQ(e.major_axis, 60);
  EXPECT_DOUBLE_EQ(e.minor_axis, 30);
  EXPECT_DOUBLE_EQ(e.rotation, 70);
  EXPECT_DOUBLE_EQ(e.angle_from, 3);
  EXPECT_DOUBLE_EQ(e.angle_to, 200);
  EXPECT_FALSE(e.include_center);

  // Grid-test both valid and invalid inputs:
  for (double cx : {-123.0, 0.0, 12.0, 0.3}) {
    for (double cy : {-3.0, 0.0, 768.0}) {
      for (double minor : {-90, 0, 20, 180}) {
        for (double major : {-123, 0, 6, 180}) {
          viren2d::Ellipse ellipse(cx, cy, major, minor);
          EXPECT_EQ(ellipse.IsValid(), (major > 0) && (minor > 0) && (major >= minor));

          EXPECT_TRUE(viren2d::eps_zero(ellipse.rotation));
          EXPECT_TRUE(viren2d::eps_zero(ellipse.angle_from));
          EXPECT_TRUE(viren2d::eps_equal(ellipse.angle_to, 360.0));

          ellipse.major_axis = 100;
          ellipse.minor_axis = 80;
          EXPECT_TRUE(ellipse.IsValid());


          ellipse += 42;
          EXPECT_DOUBLE_EQ(ellipse.cx, cx + 42);
          EXPECT_DOUBLE_EQ(ellipse.cy, cy + 42);

          ellipse -= 23;
          EXPECT_DOUBLE_EQ(ellipse.cx, cx + 19);
          EXPECT_DOUBLE_EQ(ellipse.cy, cy + 19);

          ellipse += viren2d::Vec2d(7, -9);
          EXPECT_DOUBLE_EQ(ellipse.cx, cx + 26);
          EXPECT_DOUBLE_EQ(ellipse.cy, cy + 10);

          ellipse -= viren2d::Vec2d(20, 11);
          EXPECT_DOUBLE_EQ(ellipse.cx, cx + 6);
          EXPECT_DOUBLE_EQ(ellipse.cy, cy - 1);
        }
      }
    }
  }
}
