#include <exception>

#include <gtest/gtest.h>

#include <viren2d/primitives.h>
#include <viren2d/math.h>

TEST(RectangleTest, All) {
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
        }
      }
    }
  }
}

