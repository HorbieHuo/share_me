#include "socket.h"

using namespace share_me_utils;

Socket::Socket(): m_addr(""), m_port(-1) {}

Socket::Socket(std::string addr, int port): m_addr(addr), m_port(port) {}

Socket::~Socket() {}

bool Socket::Start() {
    return true;
}

bool Socket::Config(std::string addr, int port) {
    m_addr = addr;
    m_port = port;
    return true;
}

bool Socket::Reconnect() {
    return true;
}

bool Socket::Connected() {
    return true;
}

bool Socket::Connect() {
    return true;
}

