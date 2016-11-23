#include "FileTransfer.h"

#include <iostream>

FileTransferServer* FileTransferServer::Instanse() {
    static FileTransferServer* f = NULL;
    if ( f == NULL ) {
        f = new FileTransferServer();
    }
    return f;
}

bool FileTransferServer::Config() {
    return true;
}