#include "FileTransfer.h"
#include <iostream>

using namespace FileTransfer;

FileTransferServer* FileTransferServer::inst = NULL;

FileTransferServer* FileTransferServer::Instanse() {
    if ( inst == NULL ) {
        inst = new FileTransferServer();
    }
    return inst;
}

bool FileTransferServer::Config() {
    std::cout << "hello file transfer ..." << std::endl;
    return true;
}
