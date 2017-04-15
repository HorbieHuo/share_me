#ifndef SHARED_ME_SOCKET
#define SHARED_ME_SOCKET

#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <string>
#include <cstring>

#include "isocket.h"

namespace share_me_utils {

class Socket:public ISocket {
private:

    static int INVALID_SOCKET_FD;

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
    bool Send(std::string content);

  virtual bool Start();
  virtual bool SetDataHandleFunc(DataHandleCallback func);
  virtual bool PostAcceptMsg();
  virtual bool PostSendMsg(void *data, size_t length);
  virtual bool PostRecvMsg(void *data);
  virtual bool OnRecvMsg(char *data, int length);


protected:

    bool Connect();
    bool Disconnect();

};

}


#endif // SHARED_ME_SOCKET