#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#include "../isocket.h"

namespace share_me_utils {
    class Socket: public ISocket {
        public:
            Socket();
            Socket(string ip, int port);

            void Set();

        private:

            bool init();

            SOCKET m_socketHandle;
            struct addrinfo m_addr;
            int port;
            WSADATA m_ws;
    };
}