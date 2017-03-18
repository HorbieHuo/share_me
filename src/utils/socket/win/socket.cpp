#include "socket.h"

namespace SOCKET{

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

Socket::Socket(string ip, int port, SOCKET_TYPE socketType) {
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip.c_str);
    m_addr.sin_port = htons(port);
    m_socketType = socketType;
}

void Socket::Set(SOCKET_TYPE socketType) {
    m_socketType = socketType;
}

bool Socket::init() {
    if (m_socketType >= MAX_INVALID || m_socketType <= MIN_INVALID) return false;
    if (port <= 0) return false;
    WSADATA ws;
    if ( WSAStartup(MAKEWORD(2,2), &ws) != 0 ) {
        return false;
    }
    m_socketHandle = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_socketType == CLIENT) {
        if (connect(m_socketHandle, (SOCKADDR*)&m_addr, sizeof(m_addr)) != 0) {
            return false;
        }
    } else if (m_socketType == SERVER) {
        if (SOCKET_ERROR == bind(m_sockListen, (struct sockaddr *) &m_addr, sizeof(m_addr))) {
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

bool Socket::Start() {
    if (!m_socketHandle) return false;
    IOLoop* io = IOLoop::Instance();
    if (!io) return false;
    CreateIoCompletionPort(m_socketHandle, io, (DWORD)PerHandleData, 0);
    if (m_socketType == CLIENT) {

    }
}

bool Socket::PostAcceptMsg() {
    if (m_socketType != SERVER) return false;
    LPPER_IO_DATA perIoData = new PER_IO_DATA;
    memset(&(PerIoData->overlapped), 0, sizeof(OVERLAPPED));
    perIoData->databuff.len = DATA_BUF_SIZE;
    perIoData->databuff.buf = perIoData->buffer;
    perIoData->operationType = START_ACCEPT;
    perIoData->socketForAccept = new Socket(Socket::ACCEPT);
    perIoData->socketForAccept->init();
    bool ret = lpfnAcceptEx(
        m_socketHandle, perIoData->socketForAccept->GetHandle(), perIoData->dataBuffer,
        perIoData->dataLength-((sizeof(SOCKADDR_IN)+16)*2),
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
    LPPER_IO_DATA perIoData = new PER_IO_DATA;
    memset(&(PerIoData->overlapped), 0, sizeof(OVERLAPPED));
    perIoData->databuff.len = DATA_BUF_SIZE;
    perIoData->databuff.buf = perIoData->buffer;
    memcpy(data, perIoData->buffer, length);
    perIoData->operationType = SEND;
    perIoData->socketForAccept = NULL;
    if (WSASend(m_socketHandle, &(perIoData->databuf), 1, &SendBytes, 0, &(perIoData->overlapped), NULL) == SOCKET_ERROR) {
        if (WSAGetLastError() != ERROR_IO_PENDING) return false;
    }
    return true;
}

}