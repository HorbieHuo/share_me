// A simple program that computes the square root of a number
#include "FileTransfer.h"

int main(int argc, char *argv[]) {
  FileTransfer::FileTransferServer *f =
      FileTransfer::FileTransferServer::Instanse();
  f->Config();
  return 0;
}