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

char *testStr;
char recvStr[1024];

bool DataCallback(char *data, int length) {
  if (!data || length <= 0)
    return false;
  int expectlen = length <= 100 ? length : 100;
  memcpy(recvStr, data, expectlen);
  recvStr[expectlen] = '\0';
  LOG_DEBUG("recv msg: %s", recvStr);
  return true;
}

class SocketUnittest : public testing::Test {
protected:
  virtual void SetUp() {
    std::cout << "SocketUnittest SetUp ..." << std::endl;
    m_io = NULL;
    testStr = "hello";
    memset(m_sockets, 0, sizeof(m_sockets));
    memset(recvStr, 0, sizeof(recvStr));
  }
  virtual void TearDown() {
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
    std::cout << "SocketUnittest TearDown finish..." << std::endl;
  }

  IOLoop *m_io;
  Socket *m_sockets[SOCKET_CONTINER_CAPACITY];
};

TEST_F(SocketUnittest, OneEqual) { EXPECT_EQ(1, 1); }

TEST_F(SocketUnittest, socket_send_and_recieve) {
  EXPECT_EQ(nullptr, m_io);
  m_io = IOLoop::Instanse();
  ASSERT_NE(nullptr, m_io);
  ASSERT_TRUE(m_io->Init());
  m_sockets[0] = new Socket(9999);
  m_sockets[0]->SetDataHandleFunc(DataCallback);
  ASSERT_NE(nullptr, m_sockets[0]);
  ASSERT_TRUE(m_sockets[0]->Start());
  ASSERT_TRUE(m_io->AddServerSocket(m_sockets[0]));

  m_sockets[1] = new Socket("127.0.0.1", 9999, Socket::CLIENT);
  ASSERT_NE(nullptr, m_sockets[1]);
  ASSERT_TRUE(m_sockets[1]->Start());
  ASSERT_TRUE(m_io->AddClientSocket(m_sockets[1]));
  Sleep(10);
  EXPECT_TRUE(m_sockets[1]->PostSendMsg(testStr, 5));
  Sleep(10);
  EXPECT_STREQ(testStr, recvStr);
  EXPECT_TRUE(m_sockets[1]->PostSendMsg("12345", 5));
  Sleep(10);
  EXPECT_STREQ("12345", recvStr);
  // EXPECT_TRUE(m_sockets[1]->PostSendMsg("0", 0));
  // m_sockets[1] = nullptr;
  // Sleep(10);
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
