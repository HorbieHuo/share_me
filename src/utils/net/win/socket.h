#ifndef SAHRE_ME_UTILS_SOCKET
#define SAHRE_ME_UTILS_SOCKET

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include "Mswsock.h"

#include "../isocket.h"
#include "ioloop.h"

namespace share_me_utils {

class Socket: public ISocket {
    public:
        explicit Socket();
        explicit Socket(int port);
        explicit Socket(SOCKET_TYPE socketType);
        explicit Socket(std::string ip, int port, SOCKET_TYPE socketType);
        virtual ~Socket();

        void Set(SOCKET_TYPE socketType);
		SOCKET GetHandle() { return m_socketHandle; }
        SOCKET_TYPE GetSocketType() { return m_socketType; }
        virtual bool Start();

        virtual bool SetDataHandleFunc(DataHandleCallback func);

        virtual bool PostAcceptMsg();
        virtual bool PostSendMsg(void* data, size_t length);
        virtual bool PostRecvMsg(void* data);

        virtual bool OnRecvMsg(char* data, int length);
        virtual bool OnAcceptSocket(void* data);

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
