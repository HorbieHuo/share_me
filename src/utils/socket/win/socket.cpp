#include "socket.h"

namespace SOCKET{

Socket::Socket():m_socketHandle(0), m_port(-1), m_addr({0}) {}

Socket::Socket(int port) {
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
    m_addr.sin_port = htons(port);
    m_socketType = SERVER;
}

Socket::Socket(string ip, int port, SOCKET_TYPE socketType) {
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip.c_str);
    m_addr.sin_port = htons(port);
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
    } else if {
        if (SOCKET_ERROR == bind(m_sockListen, (struct sockaddr *) &m_addr, sizeof(m_addr))) {
            return false;
        }
        if (listen(m_socketHandle, 10) != 0) return false;
    } else return false;
    return true;
}

}