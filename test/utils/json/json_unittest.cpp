#include "config.h"
#include "gtest/gtest.h"
#include <iostream>
#include <fstream>

#include "json.h"

using namespace share_me_utils;

TEST(json_unittest, two_equal_two) { EXPECT_EQ(2, 2); }

char *GetTestJsonString() {
  std::ifstream in(TEST_TILE_PATH "/json.json");
  std::string str;
  if (in.is_open()) {
    std::ostringstream tmp;
    tmp << in.rdbuf();
    str = tmp.str();

  } else {
    str = "{\"a\": \"d\", \"asss\": \"ddddd\"}";
  }
  int strLength = (int)str.length();
  char *s = new char[strLength + 1];
  memcpy(s, str.c_str(), strLength);
  s[strLength] = '\0';
  return s;
}

class JsonUnittest : public testing::Test {
protected:
  virtual void SetUp() {
    std::cout << "JsonUnittest SetUp ..." << std::endl;
    m_json = new Json();
    // m_text = "{\"a\": \"d\"}";
    m_text = GetTestJsonString();
  }
  virtual void TearDown() {
    if (m_json) {
      delete m_json;
      m_json = nullptr;
    }
    if (m_text) {
      delete[] m_text;
      m_text = nullptr;
    }
    std::cout << "JsonUnittest TearDown finish..." << std::endl;
  }

  Json *m_json;
  char *m_text;
};

TEST_F(JsonUnittest, OneEqual) { EXPECT_EQ(1, 1); }

TEST_F(JsonUnittest, json_test) {
  ASSERT_NE(m_json, nullptr);
  m_json->Set(m_text, (int)strlen(m_text));
  ASSERT_EQ(m_json->Paser(), true);
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}