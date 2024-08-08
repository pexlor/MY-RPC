#pragma once

#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include "ThreadPoll.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include "tinypb_coder.h"
#include "abstract_coder.h"
#include "rocket/common/log.h"
#include "rpc_dispatcher.h"

class RPCServer
{
private:
    TcpServer tcpserver_;
    ThreadPoll threadpoll_;
    AbstractCoder* m_coder {NULL};
public:
    RPCServer(const char * ip ,const uint16_t port,int threadnum,int workthreadnum);
    ~RPCServer();
    void Start();
    void Stop();
    void HandleNewConnction(spConnection  coon);
    void HandleCloseConnection(spConnection  conn );
    void HandleErrorConnection(spConnection  conn);
    void HandleMessage(spConnection  conn,std::string &message);
    void HandleSendComplate(spConnection  conn);
    void HandleEpollTimeout(EventLoop *loop);
    void OnMessage(spConnection  conn,std::string &message);

    void RegisterRpcService(std::shared_ptr<google::protobuf::Service> service);
};

