#ifndef SHARED_ME_SOCKET
#define SHARED_ME_SOCKET

#if defined(__unix__)
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <string>

namespace share_me_utils {

class Socket {
private:

    int m_socketInst;

    int m_port;
    std::string m_addr;

public:

    Socket();
    Socket(std::string addr, int port);
    virtual ~Socket();

    bool Config(std::string addr, int port);
    bool Start();
    bool Reconnect();
    bool Connected();



protected:

    bool Connect();

};

}


#endif // SHARED_ME_SOCKET