#pragma once
#include "Socket.h"
#include "Epoll.h"
#include "EventLoop.h"
#include "Connection.h"
#include <functional>

/**
 * @param Channel类，封装epoll的事件和事件对应的回调函数
*/
class Channel
{
private:
    int fd_ = -1; //事件对应的fd
    EventLoop* loop_ ; //在哪个EventLoop中，在这里面用于处理对应的事件

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
    
    void setreadcallback(std::function<void()> fn);
    void setclosecallback(std::function<void()> fn);
    void seterrorcallback(std::function<void()> fn);
    void setwritecallback(std::function<void()> fn);
};




