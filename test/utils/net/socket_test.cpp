#include "ioloop.h"
#include "socket.h"
#include "gtest/gtest.h"

using namespace share_me_utils;

TEST(FooTest, HandleNoneZeroInput) {
  EXPECT_EQ(2, 2);
  EXPECT_EQ(6, 3 + 3);
}

#define SOCKET_CONTINER_CAPACITY 10

class SocketUnittest : public testing::Test {
protected:
  virtual void SetUp() {
    m_io = NULL;
    memset(m_sockets, 0, sizeof(m_sockets));
  }
  virtual void TearDown() {
    m_io->Release();
    for (int i = 0; i < SOCKET_CONTINER_CAPACITY; ++i) {
      if (m_sockets[i]) {
        delete m_sockets[i];
        m_sockets[i] = NULL;
      }
      delete m_sockets;
    }
  }

  IOLoop *m_io;
  Socket *m_sockets[SOCKET_CONTINER_CAPACITY];
};

TEST_F(SocketUnittest, OneEqual) { EXPECT_EQ(1, 1); }

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
