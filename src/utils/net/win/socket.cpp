#include "socket.h"

namespace share_me_utils {

LPFN_ACCEPTEX Socket::m_acceptExFunc = nullptr;

Socket::Socket() : m_socketHandle(0), m_port(-1), m_addr({0}) {}

Socket::Socket(int port) {
  m_addr.sin_family = AF_INET;
  m_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
  m_addr.sin_port = htons(port);
  m_port = port;
  m_socketType = SERVER;
}

Socket::Socket(SOCKET_TYPE socketType) { m_socketType = socketType; }

Socket::Socket(std::string ip, int port, SOCKET_TYPE socketType) {
  m_addr.sin_family = AF_INET;
  inet_pton(AF_INET, ip.c_str(), &(m_addr.sin_addr.s_addr));
  // m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
  m_addr.sin_port = htons(port);
  m_socketType = socketType;
}

void Socket::Set(SOCKET_TYPE socketType) { m_socketType = socketType; }

bool Socket::SetDataHandleFunc(DataHandleCallback func) {
  m_dataHandleCallback = func;
  return true;
}

DataHandleCallback Socket::GetDataHandleFunc() { return m_dataHandleCallback; }

Socket::~Socket() {
  LOG_INFO("socket handle 0x%X", m_socketHandle);
  if (m_socketHandle) {
    closesocket(m_socketHandle);
  }
}

bool Socket::init() {
  if (m_socketType >= MAX_INVALID || m_socketType <= MIN_INVALID) {
    LOG_ERROR("socket type errro, %d", m_socketType);
    return false;
  }
  if (m_port <= 0 && m_socketType == SERVER) {
    LOG_ERROR("socket port errro, %d", m_port);
    return false;
  }

  WSADATA ws;
  if (WSAStartup(MAKEWORD(2, 2), &ws) != 0) {
    LOG_ERROR("WSAStartup fail, %d", GetLastError());
    return false;
  }
  m_socketHandle =
      WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
  if (INVALID_SOCKET == m_socketHandle) {
    LOG_ERROR("socket create fail, %d", WSAGetLastError());
    return false;
  }
  if (!getAcceptExFunc())
    return false;
  if (m_socketType == CLIENT) {
    if (connect(m_socketHandle, (SOCKADDR *)&m_addr, sizeof(m_addr)) != 0) {
      LOG_ERROR("socket connet fail, %d", GetLastError());
      return false;
    }
  } else if (m_socketType == SERVER) {
    if (SOCKET_ERROR ==
        bind(m_socketHandle, (struct sockaddr *)&m_addr, sizeof(m_addr))) {
      LOG_ERROR("socket bind fail, %d", GetLastError());
      return false;
    }
    if (listen(m_socketHandle, 10) != 0) {
      LOG_ERROR("socket listen fail, %d", GetLastError());
      return false;
    }
  } else if (m_socketType == ACCEPT) {
    // nothing to do
  } else {
    LOG_ERROR("socket error type fail");
    return false;
  }
  LOG_INFO("socket handle 0x%X, type = %d", m_socketHandle, m_socketType);
  return true;
}

bool Socket::getAcceptExFunc() {
  if (Socket::m_acceptExFunc)
    return true;
  GUID guidAcceptEx = WSAID_ACCEPTEX;
  // get acceptex function pointer
  DWORD dwBytes = 0;
  if (WSAIoctl(m_socketHandle, SIO_GET_EXTENSION_FUNCTION_POINTER,
               &guidAcceptEx, sizeof(guidAcceptEx), &Socket::m_acceptExFunc,
               sizeof(Socket::m_acceptExFunc), &dwBytes, NULL, NULL) == 0) {
    return true;
  }
  LOG_ERROR("get acceptEx func fail, %d", GetLastError());
  return false;
}

bool Socket::Start() {
  if (!init()) {
    LOG_ERROR("socket init fail, %d", GetLastError());
    return false;
  }
  if (!m_socketHandle) {
    LOG_ERROR("socket handle = NULL fail, %d", GetLastError());
    return false;
  }
  // IOLoop *io = IOLoop::Instanse();
  // if (!io)
  //   return false;
  // CreateIoCompletionPort((HANDLE)m_socketHandle, io, (DWORD)0, 0);
  // if (m_socketType == CLIENT) {

  // }
  return true;
}

bool Socket::PostAcceptMsg() {
  if (m_socketType != SERVER) {
    LOG_ERROR("socket type error %d", m_socketType);
    return false;
  }
  if (!Socket::m_acceptExFunc) {
    LOG_ERROR("socket acceptEx func pointer is NULL");
    return false;
  }
  DWORD dwBytes = 0;
  LPPER_IO_DATA perIoData = new PER_IO_DATA;
  memset(&(perIoData->overlapped), 0, sizeof(OVERLAPPED));
  perIoData->bufferLen = DATA_BUF_SIZE;
  perIoData->databuff.len = dwBytes;
  perIoData->databuff.buf = perIoData->buffer;
  perIoData->operationType = START_ACCEPT;
  perIoData->dataOpretedLen = 0;
  perIoData->socketForAccept = new Socket(ACCEPT);
  if (!perIoData->socketForAccept->init()) {
    LOG_ERROR("socketForAccept init fail");
    return false;
  }
  BOOL ret = Socket::m_acceptExFunc(
      m_socketHandle, perIoData->socketForAccept->GetHandle(),
      perIoData->buffer,
      perIoData->bufferLen - ((sizeof(SOCKADDR_IN) + 16) * 2),
      sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes,
      &(perIoData->overlapped));
  if (FALSE == ret && ERROR_IO_PENDING != GetLastError()) {
    LOG_ERROR("socket add accept fail, %d", GetLastError());
    return false;
  }
  LOG_INFO("socket post accept request");
  return true;
}

bool Socket::PostSendMsg(void *data, size_t length) {
  if (m_socketType == SERVER)
    return false;
  if (length >= DATA_BUF_SIZE)
    return false;
  LOG_INFO("send msg[%d]: %s", length, (char*)data);
  DWORD sendBytes = (DWORD)length;
  LPPER_IO_DATA perIoData = new PER_IO_DATA;
  memset(&(perIoData->overlapped), 0, sizeof(OVERLAPPED));
  perIoData->bufferLen = DATA_BUF_SIZE;
  perIoData->databuff.len = sendBytes;
  perIoData->databuff.buf = perIoData->buffer;
  memcpy(perIoData->buffer, data, length);
  perIoData->operationType = SEND;
  perIoData->socketForAccept = NULL;
  perIoData->dataOpretedLen = 0;
  if (WSASend(m_socketHandle, &(perIoData->databuff), 1, &sendBytes, 0,
              &(perIoData->overlapped), NULL) == SOCKET_ERROR) {
    if (WSAGetLastError() != ERROR_IO_PENDING)
      return false;
  }
  return true;
}

bool Socket::OnRecvMsg(char *data, int length) {
  if (!data)
    return false;
  if (length <= 0)
    return false;
  LOG_INFO("socket recv data 0x%X, length = %d", data, length);
  if (!m_dataHandleCallback)
    return false;
  m_dataHandleCallback(data, length);
  return true;
}

bool Socket::PostRecvMsg(void *data) {
  LPPER_IO_DATA perIoData = (LPPER_IO_DATA)data;
  if (!perIoData) {
    perIoData = new PER_IO_DATA;
  }
  DWORD recvBytes = 0;
  DWORD flags = 0;
  memset(&(perIoData->overlapped), 0, sizeof(OVERLAPPED));
  perIoData->databuff.len = DATA_BUF_SIZE;
  perIoData->databuff.buf = perIoData->buffer;
  perIoData->bufferLen = DATA_BUF_SIZE;
  // memcpy(data, perIoData->buffer, length);
  perIoData->operationType = RECV;
  perIoData->socketForAccept = NULL;
  perIoData->dataOpretedLen = 0;
  int ret = WSARecv(m_socketHandle, &(perIoData->databuff), 1, &recvBytes,
                    &flags, &(perIoData->overlapped), NULL);
  if (ret != 0) {
    if (GetLastError() != WSA_IO_PENDING) {
      LOG_ERROR("socket post recv error[%d], %d", ret, GetLastError());
      return false;
    }
  }
  LOG_INFO("socket post recv quest");
  return true;
}

bool Socket::OnAcceptSocket(void *data) {
  if (!data)
    return false;
  LOG_INFO("accept one socket");
  LPPER_IO_DATA perIoData = (LPPER_IO_DATA)data;
  if (perIoData->socketForAccept != this) {
    LOG_ERROR("socket accept object error");
    return false;
  }
  int ret = setsockopt(m_socketHandle, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                       (char *)&(m_socketHandle), sizeof(m_socketHandle));
  if (ret == SOCKET_ERROR) {
    LOG_ERROR("socket opt error, %d", GetLastError());
    return false;
  }
  IOLoop *io = IOLoop::Instanse();
  if (!io->AddAcceptedSocket(this)) {
    LOG_ERROR("socker add to io error");
    return false;
  }
  return PostRecvMsg(nullptr);
}

}