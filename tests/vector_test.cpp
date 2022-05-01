#include <exception>
#include <initializer_list>
#include <cmath>

#include <gtest/gtest.h>

#include <viren2d/primitives.h>
#include <viren2d/math.h>
//#include <viren2d/string_utils.h>
//#include <helpers/enum.h>

template<typename _Tp, int dim>
::testing::AssertionResult CheckVector(
    const viren2d::Vec<_Tp, dim> &val,
    std::initializer_list<_Tp> expect) {
  // Sanity check
  if (dim != static_cast<int>(expect.size()))
    return ::testing::AssertionFailure()
        << "Invalid number of vector elements for comparison: "
        << val.TypeName() << " has " << dim
        << ", but initializer_list has " << expect.size();

  for (int i = 0; i < dim; ++i) {
    if (!viren2d::eps_equal(val.val[i], expect.begin()[i]))
      return ::testing::AssertionFailure()
          << val.ToString() << " differs at val[" << i << "], found "
          << val.val[i] << " but expected " << expect.begin()[i];
  }
  return ::testing::AssertionSuccess();
}


template<typename _Tp, int dim>
void VectorTestHelper(viren2d::Vec<_Tp, dim> &vec) {
  EXPECT_GE(dim, 2);

  // Test indexing
  for (int i = 0; i < dim; ++i) {
    EXPECT_EQ(vec.val[dim - i - 1], vec[-(i+1)]);
  }

  // Test out-of-bounds
  EXPECT_THROW(vec[dim], std::out_of_range);
  EXPECT_THROW(vec[-dim-1], std::out_of_range);

  // Create a copy
  auto copy = viren2d::Vec<_Tp, dim>(vec);

  EXPECT_EQ(vec, copy);

  // Basic arithmetics
  auto vec2 = vec + vec;
  EXPECT_NE(vec, vec2);
  EXPECT_EQ(2 * vec, vec2);

  vec *= 2;
  EXPECT_EQ(vec, vec2);

  vec /= 2;
  EXPECT_EQ(vec2 / 2, vec);

  vec2 = vec;
  EXPECT_EQ(vec2, vec);
  EXPECT_TRUE(vec2 == copy);

  auto vec3 = vec + vec2 + copy;
  EXPECT_EQ(3 * vec, vec3);

  // Distance/Length & dot product:
  auto dot1 = vec.Dot(vec);
  auto dot3 = vec.Dot(vec3);
  EXPECT_DOUBLE_EQ(3.0 * dot1, 1.0 * dot3);

  auto len = vec.Length();
  EXPECT_DOUBLE_EQ(std::sqrt(1.0 * dot1), 1.0 * len);

  EXPECT_DOUBLE_EQ(1.0 * dot1, vec.LengthSquared());

  viren2d::Vec<_Tp, dim> zero;
  auto dist = vec.Distance(zero);
  EXPECT_DOUBLE_EQ(dist, len);

  vec2 = 4.0 * vec;
  dist = vec.Distance(vec2);
  EXPECT_DOUBLE_EQ(dist, len * 3);


  // Cross product (only for 3d)
  viren2d::Vec<_Tp, dim> other;
  for (int i = 0; i < dim; ++i)
    other[i] = static_cast<_Tp>(i);

  if (dim != 3) {
    EXPECT_THROW(vec.Cross(other), std::logic_error);
  } else {
    auto cross = vec.Cross(other);
    viren2d::Vec<_Tp, dim> expected(
          vec.y() * other.z() - vec.z() * other.y(),
          vec.z() * other.x() - vec.x() * other.z(),
          vec.x() * other.y() - vec.y() * other.x());
    EXPECT_DOUBLE_EQ(cross.x(), expected.x());
    EXPECT_DOUBLE_EQ(cross.y(), expected.y());
    EXPECT_DOUBLE_EQ(cross.z(), expected.z());
  }
}


TEST(VectorTest, All) {
  EXPECT_THROW(viren2d::Vec2d(1, 2, 3), std::invalid_argument);
  EXPECT_THROW(viren2d::Vec3d(2, 17), std::invalid_argument);
  EXPECT_THROW(viren2d::Vec4d(2, 17), std::invalid_argument);
  EXPECT_THROW(viren2d::Vec4d(2, 17, 3), std::invalid_argument);

  viren2d::Vec2d v2d_a(23, 17);
  VectorTestHelper(v2d_a);

  viren2d::Vec2d v2d_b(0.01, -9.001);
  VectorTestHelper(v2d_b);

  viren2d::Vec2d v2d_c(-735.008, -0.99);
  VectorTestHelper(v2d_c);


  viren2d::Vec3d v3d_a(1, 2, 3);
  VectorTestHelper(v3d_a);

  viren2d::Vec3d v3d_b(-0.1, 99, -15.3);
  VectorTestHelper(v3d_b);

  viren2d::Vec3d v3d_c(12.3, -0.42, 77.7);
  VectorTestHelper(v3d_c);


  viren2d::Vec2i v2i(9, -3);
  VectorTestHelper(v2i);
}
