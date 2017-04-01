#include "ioloop.h"

namespace share_me_utils {

DWORD _stdcall ServerWorkThread(LPVOID CompletionPortID);

IOLoop *IOLoop::m_io = nullptr;

IOLoop *IOLoop::Instanse() {
  if (!m_io) {
    m_io = new IOLoop();
  }
  return m_io;
}

IOLoop::IOLoop() {
  m_threadLivedCount = 0;
  memset(m_thread, 0, sizeof(m_thread));
}

IOLoop::~IOLoop() {}

void IOLoop::OnThreadClose() {
  if (m_threadLivedCount > 0)
    InterlockedExchangeAdd(&m_threadLivedCount, -1);
}

long IOLoop::GetThreadLivedCount() { return m_threadLivedCount; }

bool IOLoop::Init() {
  m_completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (!m_completionPort)
    return false;

  // SYSTEM_INFO mySysInfo;
  // GetSystemInfo(&mySysInfo );
  DWORD threadID;

  for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
    HANDLE threadHandle;
    // PTHREAD_START_ROUTINE
    threadHandle =
        CreateThread(NULL, 0, ServerWorkThread, m_completionPort, 0, &threadID);
    if (threadHandle == NULL) {
      // std::cout<< "CreateThread failed. Error:"<< GetLastError()<< std::endl;
      return false;
    }
    ++m_threadLivedCount;
    CloseHandle(threadHandle);
  }
  return true;
}

void IOLoop::Release() {
  long livedThreadCount = GetThreadLivedCount();
  for (int i = 0; i < livedThreadCount; ++i) {
    LPPER_IO_DATA perIoData = new PER_IO_DATA;
    memset(&(perIoData->overlapped), sizeof(OVERLAPPED), 0);
    perIoData->databuff.len = DATA_BUF_SIZE;
    perIoData->databuff.buf = perIoData->buffer;
    perIoData->operationType = END_THREAD;
    PostQueuedCompletionStatus(m_completionPort, (DWORD)sizeof(int),
                               (ULONG_PTR)NULL, (LPOVERLAPPED)perIoData);
  }
  while (GetThreadLivedCount())
    Sleep(10);
  if (m_io)
    delete m_io;
  m_io = NULL;
}

DWORD _stdcall ServerWorkThread(LPVOID CompletionPortID) {
  HANDLE complationPort = (HANDLE)CompletionPortID;
  DWORD bytesTransferred;
  LPPER_IO_OPERATION_DATA pIoData = NULL;
  Socket *socket = NULL;
  DWORD sendBytes = 0;
  DWORD recvBytes = 0;
  // DWORD flags;

  while (1) {
    if (GetQueuedCompletionStatus(complationPort, &bytesTransferred,
                                  (PULONG_PTR)&socket, (LPOVERLAPPED *)&pIoData,
                                  INFINITE) == 0) {
      // std::cout<< "GetQueuedCompletionStatus failed. Error:"<<
      // GetLastError()<< std::endl;
      return 0;
    }

    if (socket == NULL && pIoData->operationType == END_THREAD)
      break;

    // 检查本次是否有数据接收,如没有，是为socket结束信号
    if (bytesTransferred == 0) {
      // std::cout<< " Start closing socket..."<< std::endl;
      if (socket == NULL)
        continue;
      if (CloseHandle((HANDLE)socket->GetHandle()) == SOCKET_ERROR) {
        // std::cout<< "Close socket failed. Error:"<< GetLastError()<<
        // std::endl;
        return 0;
      }

      delete socket;
      delete pIoData;
      continue;
    }

    // if (pIoData->callback) {
    //     pIoData->callback(pIoData->databuff.buf, bytesTransferred);
    // }

    // ZeroMemory(&(pIoData->overlapped), sizeof( OVERLAPPED ));
    // pIoData->databuff.len= DataBuffSize;
    // pIoData->databuff.buf= pIoData->buffer;

    // if( WSARecv(pHandleData->socket,&(pIoData->databuff),1, &recvBytes,
    // &flags, &(pIoData->overlapped),NULL )== SOCKET_ERROR)
    // {
    //     if( WSAGetLastError() != ERROR_IO_PENDING)
    //     {
    //         // std::cout<< "WSARecv() failed. Error:"<< GetLastError()<<
    //         std::endl;
    //         return 0;
    //     }
    //     else
    //     {
    //         // std::cout<< "WSARecv() io pending"<< std::endl;
    //         return 0;
    //     }
    // }
  }
  IOLoop *io = IOLoop::Instanse();
  if (io)
    io->OnThreadClose();
  return 0;
}

bool IOLoop::AddServerSocket(Socket *socket) {
  if (!socket)
    return false;
  HANDLE socketHandle = (HANDLE)socket->GetHandle();
  CreateIoCompletionPort(socketHandle, m_completionPort, (ULONG_PTR)socket, 0);
  LPPER_IO_DATA perIoData = new PER_IO_DATA;
  memset(&(perIoData->overlapped), sizeof(OVERLAPPED), 0);
  perIoData->databuff.len = DATA_BUF_SIZE;
  perIoData->databuff.buf = perIoData->buffer;
  perIoData->operationType = START_ACCEPT;
  PostQueuedCompletionStatus(m_completionPort, (DWORD)sizeof(int),
                             (ULONG_PTR)socket, (LPOVERLAPPED)perIoData);
  return true;
}

bool IOLoop::AddClientSocket(Socket *socket) {
  // TODO 作为客户端Socket 加入io的过程
  if (!socket)
    return false;
  HANDLE socketHandle = (HANDLE)socket->GetHandle();
  CreateIoCompletionPort(socketHandle, m_completionPort, (ULONG_PTR)socket, 0);
  return true;
}
}
