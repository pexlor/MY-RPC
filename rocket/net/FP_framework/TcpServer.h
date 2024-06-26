#pragma once
#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPoll.h"
#include <memory>
#include <map>
#include <functional>
#include <mutex>

class Channel;
class Socket;
class EventLoop;
class TcpServer
{
private:
    int threadnum_;
    EventLoop *mainloop_;//这其中包含了一个主线程，由该类持有
    Acceptor acceptor_;//接收线程
    std::vector<std::unique_ptr<EventLoop>> subloops_;
    ThreadPoll threadpoll_;//IO线程池
    
    

    std::mutex mmutex;
    std::map<int ,spConnection> conns_;//管理连接线程

    std::function<void(spConnection )> newconnction_;//函数接口
    std::function<void(spConnection )> closeconnection_;
    std::function<void(spConnection )> errorconnection_;
    std::function<void(spConnection ,std::string &)> onmessage_;
    std::function<void(spConnection )> sendcomplate_;
    std::function<void(EventLoop *)>  epolltimeout_;

public:
    TcpServer(const char * ip ,uint16_t port,int threadnum);
    ~TcpServer();

    void start();
    void stop();
    void newconnction(std::unique_ptr<Socket> clientsock);
    void closeconnection(spConnection conn );
    void errorconnection(spConnection  conn);
    void onmessage(spConnection  conn,std::string &message);
    void sendcomplate(spConnection conn);
    void epolltimeout(EventLoop *loop);
    void removeconnection(int fd);

    void setnewconnction(std::function<void(spConnection)> fn);
    void setcloseconnection(std::function<void(spConnection)> fn);
    void seterrorconnection(std::function<void(spConnection)> fn);
    void setonmessage(std::function<void(spConnection,std::string &)> fn);
    void setsendcomplate(std::function<void(spConnection )> fn);
    void setepolltimeout(std::function<void(EventLoop *)> fn);

};