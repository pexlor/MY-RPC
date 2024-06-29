#pragma once
#include "Socket.h"
#include "Epoll.h"
#include "EventLoop.h"
#include "Connection.h"
#include <functional>


class EventLoop;
/*通道类，理解是对epoll中的events进行了封装，替代原来的fd为ptr指针*/
class Channel
{
private:
    int fd_ = -1;
    EventLoop* loop_ ; //非此类管理。需要调用其中的函数设置epoll
    bool inepoll_ = false;
    uint32_t events_ = 0;
    uint32_t revents_ = 0;
    std::function<void()> readcallback_;
    std::function<void()> closecallback_;
    std::function<void()> errorcallback_;
    std::function<void()> writecallback_;
public:
    Channel(EventLoop* loop,int fd);
    ~Channel();
    int fd();
    void useet(); //设置边缘触发
    void enablereading(); //监视写事件
    void disablereading(); //取消写事件
    void enablewriting(); 
    void disablewriting();
    void setinepoll();
    void setrevents(uint32_t revent);
    void disableall();
    void remove();
    bool inpoll();
    uint32_t events();
    uint32_t revents();

    void handleevent();
    //void newconnction(Socket * servsock);
    //void onmessage();
    void setreadcallback(std::function<void()> fn);
    void setclosecallback(std::function<void()> fn);
    void seterrorcallback(std::function<void()> fn);
    void setwritecallback(std::function<void()> fn);
};




