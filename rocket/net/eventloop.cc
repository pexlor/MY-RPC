
#include "eventloop.h"

namespace rocket {

static thread_local EventLoop * t_current_eventloop = NULL;

static int g_epoll_max_timeout = 10000;

EventLoop::EventLoop() {
    if(t_current_eventloop != NULL){
        //ERRORLOG("failed to create event loop");
        exit(-1);
    }
    //m_pid = getThreadId();//获取线程id
    m_epool_fd = epoll_create(10);

    if(m_epool_fd == -1){
        exit(-1);
    }

    t_current_eventloop = this;
}

EventLoop::~EventLoop(){

}

void EventLoop::loop(){
    while(!m_stop_flag){
        std::queue<std::function<void()>> tmp_tasks;
        {
            std::lock_guard<std::mutex> gd(mutex_);
            tmp_tasks = m_pending_tasks;
            m_pending_tasks.swap(tmp_tasks);
        }

        while(!tmp_tasks.empty())
        {
            tmp_tasks.front()();
            tmp_tasks.pop();
        }

        //int ret = epoll_wait(g_epoll_max_timeout);
    }
}

void EventLoop::wakeup(){

}

void EventLoop::stop(){
    
}

}