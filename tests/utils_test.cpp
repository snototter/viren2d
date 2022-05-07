#include <exception>

#include <gtest/gtest.h>

#include <viren2d/colors.h>
#include <helpers/enum.h>


enum class TestEnum : unsigned short {
  ZERO = 0,
  ONE = 1,
  TWO = 1 << 1,
  FOUR = 1 << 2,
  EIGHT = 1 << 3
};


TEST(UtilitiesTest, Enumeration) {
  auto a = TestEnum::ONE | TestEnum::TWO;
  EXPECT_EQ(static_cast<unsigned short>(a), 3);

  // OR
  EXPECT_TRUE(viren2d::IsFlagSet(a, TestEnum::ONE));
  EXPECT_TRUE(viren2d::IsFlagSet(a, TestEnum::TWO));
  EXPECT_FALSE(viren2d::IsFlagSet(a, TestEnum::FOUR));
  EXPECT_FALSE(viren2d::IsFlagSet(a, TestEnum::EIGHT));

  // OR assignment
  a |= TestEnum::EIGHT;
  EXPECT_TRUE(viren2d::IsFlagSet(a, TestEnum::ONE));
  EXPECT_TRUE(viren2d::IsFlagSet(a, TestEnum::TWO));
  EXPECT_FALSE(viren2d::IsFlagSet(a, TestEnum::FOUR));
  EXPECT_TRUE(viren2d::IsFlagSet(a, TestEnum::EIGHT));

  // AND
  EXPECT_TRUE((a & TestEnum::ZERO) == TestEnum::ZERO);
  EXPECT_TRUE((a & TestEnum::ONE) == TestEnum::ONE);
  EXPECT_TRUE((a & TestEnum::TWO) == TestEnum::TWO);
  EXPECT_TRUE((a & TestEnum::FOUR) == TestEnum::ZERO);  // Bit should not be set
  EXPECT_TRUE((a & TestEnum::EIGHT) == TestEnum::EIGHT);

  // AND assignment
  a &= TestEnum::TWO;
  EXPECT_FALSE(viren2d::IsFlagSet(a, TestEnum::ONE));
  EXPECT_TRUE(viren2d::IsFlagSet(a, TestEnum::TWO));
  EXPECT_FALSE(viren2d::IsFlagSet(a, TestEnum::FOUR));
  EXPECT_FALSE(viren2d::IsFlagSet(a, TestEnum::EIGHT));

  // NEGATION
  auto neg = ~a;
  EXPECT_TRUE(viren2d::IsFlagSet(neg, TestEnum::ONE));
  EXPECT_FALSE(viren2d::IsFlagSet(neg, TestEnum::TWO));
  EXPECT_TRUE(viren2d::IsFlagSet(neg, TestEnum::FOUR));
  EXPECT_TRUE(viren2d::IsFlagSet(neg, TestEnum::EIGHT));

  // XOR
  EXPECT_TRUE((a ^ TestEnum::ONE)   == (a |  TestEnum::ONE));
  EXPECT_TRUE((a ^ TestEnum::TWO)   == (a & ~TestEnum::TWO));  // Currently, only bit for "TWO" should be set
  EXPECT_TRUE((a ^ TestEnum::FOUR)  == (a |  TestEnum::FOUR));
  EXPECT_TRUE((a ^ TestEnum::EIGHT) == (a |  TestEnum::EIGHT));

  // XOR assignment
  a = TestEnum::ONE | TestEnum::TWO;
  a ^= TestEnum::ONE;
  EXPECT_FALSE(viren2d::IsFlagSet(a, TestEnum::ONE));
  EXPECT_TRUE(viren2d::IsFlagSet(a, TestEnum::TWO));
  EXPECT_FALSE(viren2d::IsFlagSet(a, TestEnum::FOUR));
  EXPECT_FALSE(viren2d::IsFlagSet(a, TestEnum::EIGHT));
}
