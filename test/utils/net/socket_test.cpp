#include "gtest/gtest.h"

TEST(FooTest, HandleNoneZeroInput) {
  EXPECT_EQ(2, 2);
  EXPECT_EQ(6, 3 + 3);
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
