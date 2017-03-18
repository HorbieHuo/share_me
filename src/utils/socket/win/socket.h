#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#include "../isocket.h"

namespace share_me_utils {

typedef bool (*DataHandleCallback)(char* data, int length);
class Socket: public ISocket {
    public:
        Socket();
        Socket(int port);
        Socket(SOCKET_TYPE socketType);
        Socket(string ip, int port, SOCKET_TYPE socketType);

        void Set(SOCKET_TYPE socketType);
        HANDLE GetHandle() { return m_socketHandle; }
        SOCKET_TYPE GetSocketType() { return m_socketType; }
        bool Start();

        bool SetDataHandleFunc(DataHandleCallback func);

        bool PostAcceptMsg();
        bool PostSendMsg(void* data, size_t length);
        bool PostRecvMsg();

        void OnRecvMsg(char* data, int length);

    private:

        bool init();

        SOCKET m_socketHandle;
        struct sockaddr_in m_addr;
        // WSADATA m_ws;
        SOCKET_TYPE m_socketType;
        DataHandleCallback m_dataHandleCallback;
};
}