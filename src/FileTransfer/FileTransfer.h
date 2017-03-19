#ifndef SHARE_ME_FILE_TRANSFER
#define SHARE_ME_FILE_TRANSFER

#if defined(__unix__)
#define EXPORT
#elif defined(_WIN32)
//#define EXPORT _declspec(dllimport)
#define EXPORT
#endif

#if defined(__unix__)
#define SOCKET
#elif defined(_WIN32)
#define SOCKET
#endif

#include <list>

namespace FileTransfer {

class EXPORT FileTransferServer {
    
private:

    std::list<int> socketFDs;

    FileTransferServer(){};
    ~FileTransferServer(){};

public:
    static FileTransferServer* Instanse();

    bool Config();
    bool Start();

};

};


#endif