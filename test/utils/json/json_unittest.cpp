#include "gtest/gtest.h"
#include <iostream>

#include "json.h"

using namespace share_me_utils;

TEST(json_unittest, two_equal_two) {
    EXPECT_EQ(2, 2);
}

class JsonUnittest : public testing::Test {
protected:
  virtual void SetUp() {
    std::cout << "JsonUnittest SetUp ..." << std::endl;
  }
  virtual void TearDown() {
    std::cout << "JsonUnittest TearDown finish..." << std::endl;
  }
};

TEST_F(JsonUnittest, OneEqual) { EXPECT_EQ(1, 1); }

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}