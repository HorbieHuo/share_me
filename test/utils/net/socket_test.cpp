#include "ioloop.h"
#include "socket.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace share_me_utils;

TEST(FooTest, HandleNoneZeroInput) {
  EXPECT_EQ(2, 2);
  EXPECT_EQ(6, 3 + 3);
}

#define SOCKET_CONTINER_CAPACITY 10

class SocketUnittest : public testing::Test {
protected:
  virtual void SetUp() {
    std::cout << "SocketUnittest SetUp ..." << std::endl;
    m_io = NULL;
    memset(m_sockets, 0, sizeof(m_sockets));
  }
  virtual void TearDown() {
    std::cout << "SocketUnittest TearDown ..." << std::endl;
    if (m_io) {
      m_io->Release();
      m_io = nullptr;
    }
    for (int i = 0; i < SOCKET_CONTINER_CAPACITY; ++i) {
      if (m_sockets[i]) {
        delete m_sockets[i];
        m_sockets[i] = nullptr;
      }
    }
  }

  IOLoop *m_io;
  Socket *m_sockets[SOCKET_CONTINER_CAPACITY];
};

TEST_F(SocketUnittest, OneEqual) { EXPECT_EQ(1, 1); }
TEST_F(SocketUnittest, IO_is_nullptr) { EXPECT_EQ(nullptr, m_io); }
TEST_F(SocketUnittest, io_instanse) {
  m_io = IOLoop::Instanse();
  ASSERT_NE(nullptr, m_io);
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
