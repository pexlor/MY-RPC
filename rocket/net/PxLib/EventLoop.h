#pragma once
#include "Channel.h"
#include "Epoll.h"
#include <memory>
#include <functional>
#include <sys/syscall.h>
#include <queue>
#include <mutex>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <map>
#include <atomic>
#include "Connection.h"
class Epoll;
class Channel;
class Connection;
using spConnection = std::shared_ptr<Connection>;
class EventLoop
{
private:
    int timeval_;
    int timeout_;
    std::unique_ptr<Epoll>  ep_ = nullptr; //一个事件循环一个Epoll
    std::function<void(EventLoop *)> epolltimeoutcallback_;//epoll超时回调函数

    pid_t threadid_;//线程pid
    std::queue<std::function<void()>> taskqueue_;//任务队列

    std::mutex mutex_;

    int wakeupfd_;
    std::unique_ptr<Channel> wakechannel_;
    
    int timerfd_;//定时器
    std::unique_ptr<Channel> timerchannel_;
    
    bool mainloop_;//主循环

    std::mutex mmutex_;
    std::map<int,spConnection> conns_;//管理连接

    std::function<void(int)> timeoutcallback_;//连接超时回调函数

    std::atomic_bool stop_;//停止标志
public:
    EventLoop(bool mainloop,int timeval,int timeout);
    ~EventLoop();
    void run();
    void stop();

    void closeconnection(int fd);
    void updatechannel(Channel *ch);
    void setepolltimeoutcallback(std::function<void(EventLoop *)> fn);
    void removechannel(Channel *ch);

    bool isinloopthread();
    void queueinloop(std::function<void()> fn);
    void wakeup();
    void handlewakeup();
    void handletimer();
    void settimeoutcallback(std::function<void(int)> fn);
    void newconnection(spConnection conn);
};