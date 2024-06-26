
#include "EventLoop.h"

/*class EventLoop
{
private:
    Epoll * ep_ = nullptr;
public:
    EventLoop();
    ~EventLoop();

    void run();
};*/

int createtimerfd(int sec = 5)
{
    time_t tfd = timerfd_create(CLOCK_MONOTONIC,TFD_CLOEXEC|TFD_NONBLOCK);
    struct itimerspec timeout;
    memset(&timeout,0,sizeof(timeout));
    timeout.it_value.tv_sec = sec;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(tfd,0,&timeout,0);
    return tfd;
}

EventLoop::EventLoop(bool mainloop,int timeval = 30,int timeout = 80):
                        timeval_(timeval),
                        timeout_(timeout),
                        mainloop_(mainloop),//是否为主循环
                        ep_(new Epoll),
                        wakeupfd_(eventfd(0,EFD_NONBLOCK)),
                        wakechannel_(new Channel(this,wakeupfd_)),
                        timerfd_(createtimerfd(timeval)),
                        timerchannel_(new Channel(this,timerfd_))
                        
{
    wakechannel_->setreadcallback(std::bind(&EventLoop::handlewakeup,this));
    wakechannel_->enablereading();

    timerchannel_->setreadcallback(std::bind(&EventLoop::handletimer,this));
    timerchannel_->enablereading();

    stop_ = false;
}

EventLoop::~EventLoop()
{
    
}

void EventLoop::run()
{
    threadid_ = syscall(SYS_gettid); //获取运行线程的ID
    while(!stop_)
    {
        std::vector<Channel *> rchannels = ep_->loop (-1);//获取EPOLL事件
        if(rchannels.size() == 0) //超时情况
        {
            epolltimeoutcallback_(this); //回调函数
            continue;
        }
        for(auto &ch : rchannels)
        {
            ch->handleevent();//执行ch的信息处理函数
        }
    }
}


void EventLoop::stop()
{
    stop_ = true;
    wakeup();
}

void EventLoop::updatechannel(Channel *ch)
{
    ep_->updatechannel(ch);
}

void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop *)> fn)
{
    epolltimeoutcallback_ = fn;
}

void EventLoop::removechannel(Channel *ch)
{
    ep_->removechannel(ch);
}

void EventLoop::settimeoutcallback(std::function<void(int)> fn)
{
    timeoutcallback_ = fn;
}

bool EventLoop::isinloopthread()
{
    return threadid_ == syscall(SYS_gettid);
}

void EventLoop::queueinloop(std::function<void()> fn)
{
    {
        std::lock_guard<std::mutex> gd(mutex_);
        taskqueue_.push(fn);
    }
    //printf("Wake up\n");
    wakeup();
}

void EventLoop::wakeup()
{
    uint64_t val = 1;
    write(wakeupfd_,&val,sizeof(val));
    //printf("Wake up ok\n");
}

void EventLoop::handlewakeup()
{
    uint64_t val;
    int fd = read(wakeupfd_,&val,sizeof(val));
    std::function<void()> fn;
    {
        std::lock_guard<std::mutex> gd(mutex_);
        while(taskqueue_.size()>0)
        {
            fn = std::move(taskqueue_.front());
            taskqueue_.pop();
            fn();
        }
    }
}

void EventLoop::handletimer()
{
    struct itimerspec timeout;
    memset(&timeout,0,sizeof(timeout));
    timeout.it_value.tv_sec = timeval_;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(timerfd_,0,&timeout,0);
    if(mainloop_)//为主时间循环
    {

    }else
    {
        time_t nowtime = time(0);
        for(auto aa = conns_.begin();aa!=conns_.end();)
        {
            if(aa->second->timeout(nowtime,timeout_))
            {
                aa->second->close();

                {
                    std::lock_guard<std::mutex> gd(mmutex_);
                    aa = conns_.erase(aa);
                }
                timeoutcallback_(aa->first);
            }else
            {
                aa++;
            }
        }
    }

}

void EventLoop::closeconnection(int fd)
{
    {
        std::lock_guard<std::mutex> gd(mmutex_);
        conns_.erase(fd);
    }
}

void EventLoop::newconnection(spConnection conn)
{
    {
        std::lock_guard<std::mutex> gd(mmutex_);
        conns_[conn->fd()] = conn;
    }
}