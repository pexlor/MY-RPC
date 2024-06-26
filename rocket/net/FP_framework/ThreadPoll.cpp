#include "ThreadPoll.h"

ThreadPoll::ThreadPoll(size_t threadnum,const std::string &threadtype) :stop_(false),threadtype_(threadtype)
{
    for(int i=0 ;i<threadnum ;i++)
    {
        threads_.emplace_back([this]
        {
            printf("creat thread %d\n",syscall(SYS_gettid));
            while(stop_ == false)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->mutex_);
                    this->condition_.wait(lock,[this]{
                        return ((this->stop_==true) || (this->taskqueue_.empty()==false));
                    });

                    if((this->stop_ == true)&&(this->taskqueue_.empty()==true)) return;

                    task = move(this->taskqueue_.front());
                    this->taskqueue_.pop();
                }
                //printf("thread run %d\n",syscall(SYS_gettid));
                task();
            }
        });
    }
}

ThreadPoll::~ThreadPoll()
{
    Stop();
}

void ThreadPoll::addtask(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(this->mutex_);
        taskqueue_.push(task);
    }
    condition_.notify_one();
}

size_t ThreadPoll::size()
{
    return threads_.size();
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