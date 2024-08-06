#include "ThreadPoll.h"

ThreadPoll::ThreadPoll(size_t threadnum,const std::string &threadtype,size_t maxThreadnum,size_t timeOut):
                        stop_(false),
                        threadtype_(threadtype),
                        currentThreadnum_(threadnum),
                        idleThreadnum_(threadnum),
                        minThreadnum_(threadnum),
                        timeOut_(timeOut)
{
    if(maxThreadnum <= threadnum)
    {
        maxThreadnum_ = threadnum;
    }else
    {
        maxThreadnum_ = maxThreadnum;
    }

    for(int i=0 ;i<currentThreadnum_ ;i++)
    {
        threads_.emplace_back(ThreadPoll::Threadwork);
    }
}

ThreadPoll::~ThreadPoll()
{
    Stop();
}

void ThreadPoll::addtask(std::function<void()> task)
{
    if(idleThreadnum_.load() <= 0 && currentThreadnum_.load() < maxThreadnum_) //没有空闲线程且小于最大线程数就创建一个新的线程
    {
        std::unique_lock<std::mutex> lock(this->threadMutex_);
        threads_.emplace_back(ThreadPoll::Threadwork);
        ++currentThreadnum_;
        ++idleThreadnum_;
    }

    {
        std::unique_lock<std::mutex> lock(this->mutex_);
        taskqueue_.push(task);
    }
    condition_.notify_one();
}

void ThreadPoll::Threadwork()
{
    printf("creat thread %d\n",syscall(SYS_gettid));
    while(stop_ == false)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(this->mutex_);
            this->condition_.wait_for(lock,std::chrono::seconds(180),[this]{
                return ((this->stop_==true) || (this->taskqueue_.empty()==false));
            });

            if((this->stop_ == true)&&(this->taskqueue_.empty()==true)) return; //停止且没任务

            if(this->taskqueue_.empty()==true && this->currentThreadnum_.load()> this->minThreadnum_) //超时
            {
                --this->idleThreadnum_;
                --this->currentThreadnum_;
                return;//主动结束线程
            }
            --this->idleThreadnum_;
            task = move(this->taskqueue_.front());
            this->taskqueue_.pop();
        }
        task();
        ++idleThreadnum_;
    }
}

size_t ThreadPoll::size()
{
    return currentThreadnum_.load();
}

void ThreadPoll::Stop()
{
    if(stop_){return;}
    stop_ = true;
    condition_.notify_all();
    for(std::thread &th : threads_)
    {
        th.join();
    }
}