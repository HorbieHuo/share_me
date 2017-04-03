#ifndef ISOCKET
#define ISOCKET

namespace share_me_utils {

typedef bool (*DataHandleCallback)(char *data, int length);

class ISocket {
public:
  enum SOCKET_TYPE { MIN_INVALID, CLIENT, SERVER, ACCEPT, MAX_INVALID };

  // ISocket();
  virtual ~ISocket() = 0;
  virtual bool Start() = 0;
  virtual bool SetDataHandleFunc(DataHandleCallback func) = 0;
  virtual bool PostAcceptMsg() = 0;
  virtual bool PostSendMsg(void *data, size_t length) = 0;
  virtual bool PostRecvMsg(void *data) = 0;
  virtual bool OnRecvMsg(char *data, int length) = 0;
};

}

#endif // ISOCKET