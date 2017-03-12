#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <iostream>
#include <mswsock.h>

#include "soxket.h"

namespace share_me_utils {

    #define SEND 0
    #define RECV 1
    #define ACCEPT 2


    /**
    * 结构体名称：PER_IO_DATA
    * 结构体功能：重叠I/O需要用到的结构体，临时记录IO数据
    **/
    const int DataBuffSize = 4 * 1024;
    typedef struct
    {
        OVERLAPPED overlapped;
        WSABUF databuff;
        char buffer[DataBuffSize];
        int BufferLen;
        int operationType;
        SOCKET client;
    }PER_IO_OPERATEION_DATA, *LPPER_IO_OPERATION_DATA, *LPPER_IO_DATA, PER_IO_DATA;

    /**
    * 结构体名称：PER_HANDLE_DATA
    * 结构体存储：记录单个套接字的数据，包括了套接字的变量及套接字的对应的客户端的地址。
    * 结构体作用：当服务器连接上客户端时，信息存储到该结构体中，知道客户端的地址以便于回访。
    **/
    typedef struct
    {
        SOCKET socket;
        SOCKADDR_STORAGE ClientAddr;
    }PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

    // 定义全局变量
    const int DefaultPort = 9999;
    vector < PER_HANDLE_DATA* > clientGroup;        // 记录客户端的向量组
    int g_nThread = 0;//开启线程数量
    HANDLE hThread[50];//线程句柄

    class IOLoop {
        private:
            IOLoop();
            ~IOLoop();

            int m_threadCount;
            HANDLE* m_thread;
            vector<PER_HANDLE_DATA* > m_inSockets;
            vector<PER_HANDLE_DATA* > m_outSockets;
            vector<PER_HANDLE_DATA* > m_serverSockets;
            HANDLE m_completionPort;

        public:
            static IOLoop* Instanse();

            void Run();
            bool Init(Socket* socket);

            bool AddServerSocket(Socket* socket);
            bool AddClientSocket(Socket* socket);
            DWORD WINAPI ServerWorkThread(LPVOID CompletionPortID);
    };
}