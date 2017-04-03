#ifndef SAHRE_ME_UTILS_IOLOOP
#define SAHRE_ME_UTILS_IOLOOP
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <mswsock.h>
#include <vector>
#include <log.h>

#include "socket.h"

namespace share_me_utils {

class Socket;

#define SEND ((unsigned long)1)
#define RECV ((unsigned long)1 << 1)
// #define ACCEPT ((unsigned long)1 << 2)
#define START_ACCEPT ((unsigned long)1 << 3)
#define END_THREAD ((unsigned long)1 << 4)

#define MAX_THREAD_COUNT 1

/**
* 结构体名称：PER_IO_DATA
* 结构体功能：重叠I/O需要用到的结构体，临时记录IO数据
**/
#define DATA_BUF_SIZE (4 * 1024)
typedef struct {
  OVERLAPPED overlapped;
  WSABUF databuff;
  char buffer[DATA_BUF_SIZE];
  int bufferLen;
  int operationType;
  int dataOpretedLen;
  Socket *socketForAccept;
} PER_IO_OPERATEION_DATA, *LPPER_IO_OPERATION_DATA, *LPPER_IO_DATA, PER_IO_DATA;

// #define DefaultPort  9999
// vector<Socket* > clientGroup;        // 记录客户端的向量组
// int g_nThread = 0;  //开启线程数量
// HANDLE hThread[50]; //线程句柄

class IOLoop {
private:
  IOLoop();
  ~IOLoop();

  HANDLE m_thread[MAX_THREAD_COUNT];
  HANDLE m_completionPort;
  long m_threadLivedCount;
  static IOLoop* m_io;

public:
  static IOLoop *Instanse();
  
  void OnThreadClose();
  long GetThreadLivedCount();

  bool Init();
  void Release();

  bool AddServerSocket(Socket *socket);
  bool AddClientSocket(Socket *socket);
  bool AddAcceptedSocket(Socket* socket);

  // static DWORD ServerWorkThread(LPVOID CompletionPortID);
};
}

#endif // SAHRE_ME_UTILS_IOLOOP