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
/**
 * @param 负责创建监听
*/
class Acceptor
{
private:
    EventLoop* loop_;//在哪个EventLoop中
    Socket servsock_;//监听套接字
    Channel acceptchannel_;//对应的事件类
    std::function<void(std::unique_ptr<Socket>)> newconnectioncb_;//新连接回调函数
public:
    Acceptor(EventLoop* loop,const std::string & ip , uint16_t port);//构造函数
    ~Acceptor();//西沟函数
    void setnewconnectioncb(std::function<void(std::unique_ptr<Socket>)> fn);//设置新连接回调函数
    void newconnction();//新链接处理函数
};


