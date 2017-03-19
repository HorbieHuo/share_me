#ifndef SAHRE_ME_UTILS_SOCKET
#define SAHRE_ME_UTILS_SOCKET

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#include "../isocket.h"
#include "ioloop.h"

namespace share_me_utils {

typedef bool (*DataHandleCallback)(char* data, int length);

class Socket: public ISocket {
    public:
        Socket();
        Socket(int port);
        Socket(SOCKET_TYPE socketType);
        Socket(std::string ip, int port, SOCKET_TYPE socketType);

        void Set(SOCKET_TYPE socketType);
		SOCKET GetHandle() { return m_socketHandle; }
        SOCKET_TYPE GetSocketType() { return m_socketType; }
        bool Start();

        bool SetDataHandleFunc(DataHandleCallback func);

        bool PostAcceptMsg();
        bool PostSendMsg(void* data, size_t length);
        bool PostRecvMsg();

        void OnRecvMsg(char* data, int length);

    private:

        bool init();
        bool getAcceptExFunc();

        SOCKET m_socketHandle;
        struct sockaddr_in m_addr;
        int m_port;
        SOCKET_TYPE m_socketType;
        DataHandleCallback m_dataHandleCallback;

        static LPFN_ACCEPTEX m_acceptExFunc;
};
}


#endif // !SAHRE_ME_UTILS_SOCKET
