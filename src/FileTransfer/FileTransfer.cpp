#include "FileTransfer.h"
#include <iostream>

#if defined(__unix__)
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

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

bool FileTransferServer::Start() {
    return true;
}
