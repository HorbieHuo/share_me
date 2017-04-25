#include "ioloop.h"
#include <iostream>

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

IOLoop::~IOLoop() {
  if (m_completionPort) {
    CloseHandle(m_completionPort);
    m_completionPort = NULL;
  }
}

void IOLoop::OnThreadClose() {
  if (m_threadLivedCount > 0)
    InterlockedExchangeAdd(&m_threadLivedCount, -1);
  LOG_INFO("thread %ld destroy", m_threadLivedCount);
}

long IOLoop::GetThreadLivedCount() { return m_threadLivedCount; }

bool IOLoop::Init() {
  m_threadLivedCount = 0;
  m_completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (!m_completionPort)
    return false;

  // SYSTEM_INFO mySysInfo;
  // GetSystemInfo(&mySysInfo );
  DWORD threadID;

  for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
    HANDLE threadHandle;
    threadHandle =
        CreateThread(NULL, 0, ServerWorkThread, m_completionPort, 0, &threadID);
    if (threadHandle == NULL) {
      LOG_ERROR("CreateThread failed, %d", WSAGetLastError());
      return false;
    }
    ++m_threadLivedCount;
    LOG_INFO("thread %ld created", m_threadLivedCount);
    CloseHandle(threadHandle);
  }
  return true;
}

void IOLoop::Release() {
  long livedThreadCount = GetThreadLivedCount();
  for (long i = 0; i < livedThreadCount; ++i) {
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
  LOG_INFO("IOLoop resource release success");
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
    bytesTransferred = 0;
    pIoData = NULL;
    socket = NULL;
    if (GetQueuedCompletionStatus(complationPort, &bytesTransferred,
                                  (PULONG_PTR)&socket, (LPOVERLAPPED *)&pIoData,
                                  INFINITE) == 0) {
      LOG_ERROR("GetQueuedCompletionStatus error, %d", WSAGetLastError());
      LOG_INFO("socket 0x%X  pIoData 0x%X", socket, pIoData);
      if (pIoData && socket) {
        if (socket->GetSocketType() == Socket::ACCEPT) {
          LOG_INFO("socket handle 0x%X", socket->GetHandle());
          delete socket;
          delete pIoData;
        }
        continue;
      }
      break;
    }

    if (socket == NULL && pIoData->operationType == END_THREAD) {
      LOG_INFO("thread end with outof loop");
      break;
    }

    if (bytesTransferred == 0) {
      LOG_INFO("socket accept close, 0x%X", socket->GetHandle());
      delete pIoData;
      delete socket;
      continue;
    }

    switch (pIoData->operationType) {
    case SEND: {
      LOG_INFO("IOCP send event, bytesTransferred = %d, sockettype = %d",
               bytesTransferred, socket->GetSocketType());
      pIoData->dataOpretedLen += bytesTransferred;
      if (pIoData->dataOpretedLen == pIoData->databuff.len) {
        LOG_INFO("IOCP msg[%d] send OK databuff.len = %d",
                 pIoData->dataOpretedLen, pIoData->databuff.len);
        delete pIoData;
      } else {
        pIoData->databuff.buf = pIoData->buffer + pIoData->dataOpretedLen;
        pIoData->databuff.len -= pIoData->dataOpretedLen;
        if (WSASend(socket->GetHandle(), &(pIoData->databuff), 1,
                    &bytesTransferred, 0, &(pIoData->overlapped),
                    NULL) == SOCKET_ERROR) {
          if (WSAGetLastError() != ERROR_IO_PENDING) {
            LOG_ERROR("can not send, socket exit");
            delete socket;
            delete pIoData;
          }
        }
      }
      break;
    }
    case RECV: {
      LOG_INFO("IOCP recv event, bytesTransferred = %d, sockettype = %d",
               bytesTransferred, socket->GetSocketType());
      if (socket->OnRecvMsg(pIoData->databuff.buf, bytesTransferred)) {
        if (!socket->PostRecvMsg(pIoData)) {
          LOG_ERROR("can not recv, socket exit");
          delete socket;
          delete pIoData;
        }
      } else {
        delete socket;
        delete pIoData;
      }
      break;
    }
    case START_ACCEPT: {
      LOG_INFO("IOCP accept event, bytesTransferred = %d, sockettype = %d",
               bytesTransferred, socket->GetSocketType());
      socket->PostAcceptMsg();
      if (!pIoData->socketForAccept)
        break;
      Socket *acceptedSocket = pIoData->socketForAccept;
      if (acceptedSocket->OnAcceptSocket(pIoData)) {
        acceptedSocket->SetDataHandleFunc(socket->GetDataHandleFunc());
        if (bytesTransferred > 0) {
          if (acceptedSocket->OnRecvMsg(pIoData->databuff.buf,
                                        bytesTransferred)) {
            LOG_ERROR("first deal accept data fail, %d", WSAGetLastError());
          }
        }
        // acceptedSocket->PostRecvMsg(nullptr);
      }
      delete pIoData;
      break;
    }
    case END_THREAD: {
      LOG_ERROR("END_THREAD msg error");
      break;
    }
    default: {
      LOG_ERROR("unkwon type = %d", pIoData->operationType);
      break;
    }
    }
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
  HANDLE ret = CreateIoCompletionPort(socketHandle, m_completionPort,
                                      (ULONG_PTR)socket, 0);
  if (!ret) {
    LOG_ERROR("add server socket error, %d", WSAGetLastError());
    return false;
  }
  return socket->PostAcceptMsg();
}

bool IOLoop::AddClientSocket(Socket *socket) {
  // TODO 作为客户端Socket 加入io的过程
  if (!socket)
    return false;
  HANDLE socketHandle = (HANDLE)socket->GetHandle();
  HANDLE ret = CreateIoCompletionPort(socketHandle, m_completionPort,
                                      (ULONG_PTR)socket, 0);
  if (!ret) {
    LOG_ERROR("add client socket error, %d", WSAGetLastError());
    return false;
  }
  return socket->PostRecvMsg(nullptr);
}

bool IOLoop::AddAcceptedSocket(Socket *socket) {
  if (!socket)
    return false;
  HANDLE socketHandle = (HANDLE)socket->GetHandle();
  HANDLE ret = CreateIoCompletionPort(socketHandle, m_completionPort,
                                      (ULONG_PTR)socket, 0);
  if (!ret) {
    LOG_ERROR("add accept socket error, %d", WSAGetLastError());
    return false;
  }
  return true;
}
}
