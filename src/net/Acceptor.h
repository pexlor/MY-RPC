#pragma once
#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Connection.h"
#include <functional>
#include <memory>
#include <common/log.h>
class Acceptor
{
private:
    EventLoop* loop_;
    Socket servsock_;
    Channel acceptchannel_;
    std::function<void(std::unique_ptr<Socket>)> newconnectioncb_;
public:
    Acceptor(EventLoop* loop,const std::string & ip , uint16_t port);
    ~Acceptor();
    void setnewconnectioncb(std::function<void(std::unique_ptr<Socket>)> fn);
    void newconnction();
};


