#include "socket.h"

namespace SOCKET{

Socket::Socket():m_socketHandle(0), m_port(-1), m_addr({0}) {}

Socket::Socket(string ip, int port) {
    m_port = port;
    init();
}

bool Socket::init() {
    struct addrinfo* result = NULL;

    if ( WSAStartup(MAKEWORD(2,2), &m_ws) != 0 ) {
        return false;
    }
    m_addr.ai_family = AF_INET;
    m_addr.ai_socktype = SOCK_STREAM;
    m_addr.ai_protocol = IPPROTO_TCP;
    m_addr.ai_flags = AI_PASSIVE;
    int iResult = getaddrinfo(NULL, m_port, &m_addr, &result);
    if ( iResult != 0 ) {
        WSACleanup();
        return false;
    }
    m_socketHandle = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (m_socketHandle == INVALID_SOCKET) {
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }
    iResult = bind( m_socketHandle, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        freeaddrinfo(result);
        closesocket(m_socketHandle);
        WSACleanup();
        return false;
    }
    freeaddrinfo(result);
    iResult = listen(m_socketHandle, 100);
    if (iResult == SOCKET_ERROR) {
        closesocket(m_socketHandle);
        WSACleanup();
        return false;
    }
    return true;
}

}