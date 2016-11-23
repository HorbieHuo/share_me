#ifndef SHARE_ME_FILE_TRANSFER
#define SHARE_ME_FILE_TRANSFER

class FileTransferServer {
private:
    FileTransferServer(){};
    ~FileTransferServer(){};

public:
    static FileTransferServer* Instanse();

    bool Config();

};

#endif