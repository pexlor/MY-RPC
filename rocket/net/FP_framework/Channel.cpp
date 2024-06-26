#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"
#include <iostream>

Channel::Channel(EventLoop* loop,int fd):loop_(loop),fd_(fd)
{

}
Channel::~Channel()
{

}

int Channel::fd()
{
    return fd_;
}

void Channel::useet() //设置边缘触发
{
    events_ |= EPOLLET;
}

void Channel::enablereading() //监视写事件
{
    events_ |= EPOLLIN;
    //printf("5\n");
    loop_->updatechannel(this);
}

void Channel::setinepoll()
{
    inepoll_ = true;
}

void Channel::setrevents(uint32_t revent)
{
    revents_ = revent;
}

bool Channel::inpoll()
{
    return inepoll_;
}

uint32_t Channel::events()
{
    return events_;
}

uint32_t Channel::revents()
{
    return revents_;
}

void Channel::setreadcallback(std::function<void()> fn) 
{
    readcallback_ = fn;
}

void Channel::handleevent()
{
    if(revents() & EPOLLRDHUP)
    {
        remove();
        closecallback_();
    }
    else if(revents() & (EPOLLIN|EPOLLPRI)) // 接收到数据
    {
        readcallback_();
    }else if(revents() & EPOLLOUT)
    {
        writecallback_();
    }else
    {
        errorcallback_();
    }
}

void Channel::setclosecallback(std::function<void()> fn)
{
    closecallback_ = fn;
    
}
void Channel::seterrorcallback(std::function<void()> fn)
{
    errorcallback_ = fn;
}

void Channel::setwritecallback(std::function<void()> fn)
{
    writecallback_ = fn;
}

void Channel::disablereading()
{
    events_ &= ~EPOLLIN;
   // printf("1\n");
    loop_->updatechannel(this);
}

void Channel::enablewriting()
{
    events_ |= EPOLLOUT;
   // printf("2\n");
    loop_->updatechannel(this);
}

void Channel::disablewriting()
{
    events_ &= ~EPOLLOUT;
    //printf("3\n");
    loop_->updatechannel(this);
}

void Channel::disableall()
{
    events_ = 0;
   // printf("4\n");
    loop_->updatechannel(this);
}

void Channel::remove()
{
    disableall();
    loop_->removechannel(this);
}