#include "ioloop.h"

namespace share_me_utils{

static IOLoop* IOLoop::Instanse() {
    static IOLoop* inst = NULL;
    if (!inst) {
        inst = new IOLoop();
    }
    return inst;
}

IOLoop::IOLoop() {
    m_threadCount = 2;
    m_thread = new HANDLE[m_threadCount];
}

IOLoop::~IOLoop() {
    if (m_thread) {
        delete[] m_thread;
        m_thread = NULL;
    }
}

bool IOLoop::Init() {
    m_completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL, 0, 0);
    if (!m_completionPort) return false;

    // SYSTEM_INFO mySysInfo;
    // GetSystemInfo(&mySysInfo );
    DWORD threadID;

    for( DWORDi =0; i < m_threadCount; ++i )
    {
        HANDLE threadHandle;
        threadHandle= CreateThread(NULL, 0, ServerWorkThread,completionPort,0, &threadID );
        if( threadHandle== NULL)
        {
            // std::cout<< "CreateThread failed. Error:"<< GetLastError()<< std::endl;
            return false;
        }
        m_thread[i] = threadHandle;
    }
    return true;
}

static DWORD IOLoop::ServerWorkThread(LPVOID CompletionPortID) {
    HANDLE complationPort =(HANDLE)CompletionPortID;
    DWORD bytesTransferred;
    LPPER_HANDLE_DATA pHandleData =NULL;
    LPPER_IO_OPERATION_DATA pIoData =NULL;
    DWORD sendBytes =0;
    DWORD recvBytes =0;
    DWORD flags;
 
    while( 1)
    {
        if( GetQueuedCompletionStatus(
            complationPort,
            &bytesTransferred,
            (PULONG_PTR)&pHandleData,
            (LPOVERLAPPED*)&pIoData,
            INFINITE ) == 0 )
        {
            // std::cout<< "GetQueuedCompletionStatus failed. Error:"<< GetLastError()<< std::endl;
            return 0;
        }
 
        // 检查本次是否有数据接收,如没有，是为程序结束信号，则结束本线程
        if( bytesTransferred == 0)
        {
            // std::cout<< " Start closing socket..."<< std::endl;
            if( CloseHandle((HANDLE)pHandleData->socket) == SOCKET_ERROR )
            {
                // std::cout<< "Close socket failed. Error:"<< GetLastError()<< std::endl;
                return 0;
            }
 
            GlobalFree(pHandleData);
            GlobalFree(pIoData);
            continue;
        }

        if (pIoData->callback) {
            pIoData->callback(pIoData->databuff.buf, bytesTransferred);
        }
        
        ZeroMemory(&(pIoData->overlapped), sizeof( OVERLAPPED ));
        pIoData->databuff.len= DataBuffSize;
        pIoData->databuff.buf= pIoData->buffer;

        if( WSARecv(pHandleData->socket,&(pIoData->databuff),1, &recvBytes, &flags, &(pIoData->overlapped),NULL )== SOCKET_ERROR)
        {
            if( WSAGetLastError() != ERROR_IO_PENDING)
            {
                // std::cout<< "WSARecv() failed. Error:"<< GetLastError()<< std::endl;
                return 0;
            }
            else
            {
                // std::cout<< "WSARecv() io pending"<< std::endl;
                return 0;
            }
        }
    }
}

void IOLoop::AddServerSocket(Socket* socket) {
    if (!socket) return ;
    HANDLE socketHandle = socket->GetHandle();
    CreateIoCompletionPort(socketHandle, m_completionPort, (DWORD)socket, 0);
    LPPER_IO_DATA perIoData = new PER_IO_DATA;
    memset(&(PerIoData->overlapped), sizeof(OVERLAPPED), 0);
    perIoData->databuff.len = DATA_BUF_SIZE;
    perIoData->databuff.buf = perIoData->buffer;
    perIoData->operationType = START_ACCEPT;
    PostQueuedCompletionStatus(m_completionPort, (DWORD)sizeof(int), (DWORD)socket, (LPOVERLAPPED)perIoData);
}

bool IOLoop::AddClientSocket(Socket* socket) {
    // TODO 作为客户端Socket 加入io的过程
}

}

