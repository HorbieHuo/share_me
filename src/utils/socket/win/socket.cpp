#include "socket.h"

namespace share_me_utils {

Socket::Socket():m_socketHandle(0), m_port(-1), m_addr({0}) {}

Socket::Socket(int port) {
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
    m_addr.sin_port = htons(port);
    m_socketType = SERVER;
}

Socket::Socket(SOCKET_TYPE socketType) {
    m_socketType = socketType;
}

Socket::Socket(std::string ip, int port, SOCKET_TYPE socketType) {
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    m_addr.sin_port = htons(port);
    m_socketType = socketType;
}

void Socket::Set(SOCKET_TYPE socketType) {
    m_socketType = socketType;
}

bool Socket::SetDataHandleFunc(DataHandleCallback func) {
	m_dataHandleCallback = func;
	return true;
}

bool Socket::init() {
    if (m_socketType >= MAX_INVALID || m_socketType <= MIN_INVALID) return false;
    if (m_port <= 0) return false;

    WSADATA ws;
    if ( WSAStartup(MAKEWORD(2,2), &ws) != 0 ) {
        return false;
    }
    m_socketHandle = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    getAcceptExFunc();
    if (m_socketType == CLIENT) {
        if (connect(m_socketHandle, (SOCKADDR*)&m_addr, sizeof(m_addr)) != 0) {
            return false;
        }
    } else if (m_socketType == SERVER) {
        if (SOCKET_ERROR == bind(m_socketHandle, (struct sockaddr *) &m_addr, sizeof(m_addr))) {
            return false;
        }
        if (listen(m_socketHandle, 10) != 0) return false;
    } else if (m_socketType == ACCEPT) {
        // nothing to do
    } else {
        return false;
    }
    return true;
}

bool Socket::getAcceptExFunc() {
    if (!m_acceptExFunc) return true;
    GUID guidAcceptEx = WSAID_ACCEPTEX;
     //get acceptex function pointer
     DWORD dwBytes = 0;
     if(WSAIoctl(
         m_socketHandle,SIO_GET_EXTENSION_FUNCTION_POINTER,
         &guidAcceptEx,sizeof(guidAcceptEx),&m_acceptExFunc,sizeof(m_acceptExFunc),
         &dwBytes,NULL,NULL)==0) {
        return true;
    }
    return false;
}

bool Socket::Start() {
    if (!m_socketHandle) return false;
    IOLoop* io = IOLoop::Instanse();
    if (!io) return false;
    CreateIoCompletionPort((HANDLE)m_socketHandle, io, (DWORD)0, 0);
    // if (m_socketType == CLIENT) {

    // }
    return true;
}

bool Socket::PostAcceptMsg() {
    if (m_socketType != SERVER) return false;
    if (!m_acceptExFunc) return false;
	DWORD dwBytes = 0;
    LPPER_IO_DATA perIoData = new PER_IO_DATA;
    memset(&(perIoData->overlapped), 0, sizeof(OVERLAPPED));
    perIoData->databuff.len = DATA_BUF_SIZE;
    perIoData->databuff.buf = perIoData->buffer;
    perIoData->operationType = START_ACCEPT;
    perIoData->socketForAccept = new Socket(ACCEPT);
    perIoData->socketForAccept->init();
    bool ret = m_acceptExFunc(
        m_socketHandle, perIoData->socketForAccept->GetHandle(), perIoData->buffer,
        perIoData->bufferLen-((sizeof(SOCKADDR_IN)+16)*2),
        sizeof(SOCKADDR_IN)+16,sizeof(SOCKADDR_IN)+16,&dwBytes,
        &(perIoData->overlapped)
    );
    if(false == ret && ERROR_IO_PENDING != GetLastError()){
        return false;
    }
    return true;
}

bool Socket::PostSendMsg(void* data, size_t length) {
    if (m_socketType == SERVER) return false;
    if (length >= DATA_BUF_SIZE) return false;
	DWORD sendBytes = length;
    LPPER_IO_DATA perIoData = new PER_IO_DATA;
    memset(&(perIoData->overlapped), 0, sizeof(OVERLAPPED));
    perIoData->databuff.len = DATA_BUF_SIZE;
    perIoData->databuff.buf = perIoData->buffer;
    memcpy(data, perIoData->buffer, length);
    perIoData->operationType = SEND;
    perIoData->socketForAccept = NULL;
    if (WSASend(m_socketHandle, &(perIoData->databuff), 1, &sendBytes, 0, &(perIoData->overlapped), NULL) == SOCKET_ERROR) {
        if (WSAGetLastError() != ERROR_IO_PENDING) return false;
    }
    return true;
}

void Socket::OnRecvMsg(char* data, int length) {
    if (!data) return;
    if (length <= 0) return;
    if (!m_dataHandleCallback) return;
    m_dataHandleCallback(data, length);
}

bool Socket::PostRecvMsg(LPPER_IO_DATA perIoData = NULL) {
    if (!perIoData) {
        perIoData = new LPPER_IO_DATA;
    }
    DWORD recvBytes = 0;
    memset(&(perIoData->overlapped), 0, sizeof(OVERLAPPED));
    perIoData->databuff.len = DATA_BUF_SIZE;
    perIoData->databuff.buf = perIoData->buffer;
    memcpy(data, perIoData->buffer, length);
    perIoData->operationType = RECV;
    perIoData->socketForAccept = NULL;
    int ret = WSARecv(
        m_socketHandle, 
        &(perIoData->databuff), 1, 
        &RecvBytes, &Flags, 
        &(perIoData->overlapped), NULL);
    if (ret != 0) {
        if (ret != WSA_IO_PENDING) {
            return false;
        }
    }
	return true;
}

}