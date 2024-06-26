#include "Epoll.h"

Epoll::Epoll()
{
    epollfd_ = epoll_create(1);
    if(epollfd_ < 0)
    {
        printf("epoll create failed %d\n",epollfd_);
        exit(-1);
    }
}
Epoll::~Epoll()
{
    close(epollfd_);
}

void Epoll::addfd(int fd,uint32_t op)
{
    struct epoll_event epollev;
    epollev.data.fd = fd;
    epollev.events = op;
    if(epoll_ctl(epollfd_,EPOLL_CTL_ADD,fd,&epollev) == -1)
    {
        perror("epoll add error\n");
        //printf("epoll add error\n");
        exit(-1);
    }
}

void Epoll::updatechannel(Channel *ch)
{
    struct epoll_event epollev;
    epollev.data.ptr = ch;
    epollev.events = ch->events();

    if(ch->inpoll())
    {
        if(epoll_ctl(epollfd_,EPOLL_CTL_MOD,ch->fd(),&epollev) == -1)
        {
            printf("epoll MOD error\n");
            //printf("epoll add sserror %d\n",errno);
            //exit(-1);
        }
    }else
    {
        if(epoll_ctl(epollfd_,EPOLL_CTL_ADD,ch->fd(),&epollev) == -1)
        {
            //printf("epoll add error\n");
            printf("epoll add error %d %d\n",errno,ch->fd());
            //exit(-1);
        }
        ch->setinepoll();
    }
    
}
std::vector<Channel *> Epoll::loop(int timeout = -1)
{
    std::vector<Channel *> rchannel;
    bzero(evs_,sizeof(evs_));
    int infds = epoll_wait(epollfd_,evs_,MaxEvents,timeout);
    if(infds < 0)
    {
        perror("epoll_wait failed");
        exit(-1);
    }
    if(infds == 0)
    {
        printf("epoll_wait timeout \n");
        return rchannel;
    }
    for(int i = 0;i<infds;i++)
    {
        Channel *ch = (Channel *) evs_[i].data.ptr;
        //printf("%x\n",ch);
        ch->setrevents(evs_[i].events);
        rchannel.push_back(ch);
    }
    return rchannel;
}

void Epoll::removechannel(Channel *ch)
{
    if(ch->inpoll())
    {
        if(epoll_ctl(epollfd_,EPOLL_CTL_DEL,ch->fd(),0) == -1)
        {
            printf("epoll EDL error %d\n",errno);
        }
    }
}