#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <strings.h>
#include <sys/epoll.h>
#include <vector>
#include "Channel.h"

/*对epoll的相关操作进行了封装，添加，修改，等待获取可用fd*/
class Channel;

class Epoll
{
private:
    /* data */
    static const int MaxEvents = 100;
    int epollfd_ = -1;
    struct epoll_event evs_[MaxEvents];
public:
    Epoll();
    ~Epoll();
    void addfd(int fd,uint32_t op);
    void updatechannel(Channel *ch);
    void removechannel(Channel *ch);
    std::vector<Channel *> loop(int timeout);
};