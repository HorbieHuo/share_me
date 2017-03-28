#ifndef ISOCKET
#define ISOCKET

namespace share_me_utils {

typedef bool (*DataHandleCallback)(char *data, int length);

class ISocket {
public:
  enum SOCKET_TYPE { MIN_INVALID, CLIENT, SERVER, ACCEPT, MAX_INVALID };
};
}

#endif // ISOCKET