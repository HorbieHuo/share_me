#ifndef SHARE_ME_FILE_TRANSFER
#define SHARE_ME_FILE_TRANSFER

#if defined(__unix__)
#define EXPORT
#elif defined(_WIN32)
#define EXPORT _declspec(dllimport)
#endif

class EXPORT FileTransferServer {
private:
    FileTransferServer(){};
    ~FileTransferServer(){};

public:
    static FileTransferServer* Instanse();

    bool Config();

};

#endif