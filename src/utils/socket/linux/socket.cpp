#include "socket.h"

using namespace share_me_utils;

int Socket::INVALID_SOCKET_FD = -1;

Socket::Socket(): m_addr(""), m_port(-1), m_socketInst() {}

Socket::Socket(std::string addr, int port): m_addr(addr), m_port(port) {}

Socket::~Socket() {
    Disconnect();
}

bool Socket::Start() {
    return Connect();
}

bool Socket::Config(std::string addr, int port) {
    if (Connected()) return false;
    m_addr = addr;
    m_port = port;
    return true;
}

bool Socket::Reconnect() {
    Disconnect();
    return Connect();
}

bool Socket::Connected() {
    if (m_socketInst < 0) return false;
    return true;
}

bool Socket::Connect() {
    if (m_socketInst != Socket::INVALID_SOCKET_FD) return false;
    m_socketInst = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socketInst < 0) return false;
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(m_port);
    inet_pton(AF_INET, m_addr.c_str(), &serverAddr.sin_addr);
    int isSuccess = connect(m_socketInst, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (isSuccess < 0) return false;
    return true;
}

bool Socket::Disconnect() {
    if (m_socketInst < 0) return true;
    close(m_socketInst);
    m_socketInst = -1;
    return true;
}

bool Socket::Send(std::string content) {
    if (!Connected()) return false;
    int isSuccess = send(m_socketInst, content.c_str(), content.length(), 0);
    if (isSuccess < 0) return false;
    return true;
}

