#include <exception>

#include <gtest/gtest.h>

#include <viren2d/primitives.h>
#include <viren2d/math.h>

TEST(RectangleTest, Initialization) {
  auto r = viren2d::Rect();
  EXPECT_FALSE(r.IsValid());

  r = viren2d::Rect({100, 400}, {80, 60}, 70, 3);
  EXPECT_TRUE(r.IsValid());
  EXPECT_DOUBLE_EQ(r.cx, 100);
  EXPECT_DOUBLE_EQ(r.cy, 400);
  EXPECT_DOUBLE_EQ(r.width, 80);
  EXPECT_DOUBLE_EQ(r.height, 60);
  EXPECT_DOUBLE_EQ(r.angle, 70);
  EXPECT_DOUBLE_EQ(r.radius, 3);

  // Grid-test both valid and invalid inputs:
  //TODO also test angles and radii
  for (double cx : {-123.0, 0.0, 12.0, 0.3}) {
    for (double cy : {-3.0, 0.0, 768.0}) {
      for (double width : {-90, 0, 20}) {
        for (double height : {-123, 0, 6, 180}) {
          viren2d::Rect rect(cx, cy, width, height);
          EXPECT_EQ(rect.IsValid(), (width > 0) && (height > 0));

          EXPECT_DOUBLE_EQ(rect.half_width(), width / 2.0);
          EXPECT_DOUBLE_EQ(rect.half_height(), height / 2.0);

          EXPECT_TRUE(viren2d::eps_zero(rect.angle));
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
        }
      }
    }
  }
}
