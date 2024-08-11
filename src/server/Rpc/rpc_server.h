#pragma once

#include "src/net/TcpServer.h"
#include "src/net/EventLoop.h"
#include "src/net/Connection.h"
#include "src/common/ThreadPoll.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include "coder/tinypb_coder.h"
#include "coder/abstract_coder.h"
#include "src/common/log.h"
#include "rpc_dispatcher.h"
#include "common/zookeeperutil.h"

class RPCServer
{
private:
    TcpServer tcpserver_;
    ThreadPoll threadpoll_;
    AbstractCoder* m_coder {NULL};
public:
    RPCServer(const char * ip ,const uint16_t port = 0,int threadnum = 0,int workthreadnum = 0);
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

