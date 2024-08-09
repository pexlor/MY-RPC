#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include "ThreadPoll.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
class EchoServer
{
private:
    TcpServer tcpserver_;
    ThreadPoll threadpoll_;
    
public:
    EchoServer(const char * ip ,const uint16_t port,int threadnum,int workthreadnum);
    ~EchoServer();
    void Start();
    void Stop();
    void HandleNewConnction(spConnection  coon);
    void HandleCloseConnection(spConnection  conn );
    void HandleErrorConnection(spConnection  conn);
    void HandleMessage(spConnection  conn,std::string &message);
    void HandleSendComplate(spConnection  conn);
    void HandleEpollTimeout(EventLoop *loop);
    void OnMessage(spConnection  conn,std::string &message);
};

