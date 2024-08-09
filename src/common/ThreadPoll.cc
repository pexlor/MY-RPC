#include "ThreadPoll.h"

ThreadPoll::ThreadPoll(size_t minthreadnum,const std::string &threadtype,size_t maxThreadnum,size_t timeOut):
                        stop_(false),
                        threadtype_(threadtype),
                        currentThreadnum_(minthreadnum),
                        idleThreadnum_(minthreadnum),
                        minThreadnum_(minthreadnum),
                        timeOut_(timeOut),
                        clearThread([this]{this->clearDeadThread();})
{
    if(maxThreadnum <= minthreadnum)
    {
        maxThreadnum_ = minthreadnum;
    }else
    {
        maxThreadnum_ = maxThreadnum;
    } 
    for(int i=0 ;i<currentThreadnum_ ;i++)
    {
        std::thread t([this]{this->Threadwork();});
        threads_[t.get_id()] = std::move(t);
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
        if(taskqueue_.size() >= idleThreadnum_.load() && currentThreadnum_.load() < maxThreadnum_) //没有空闲线程且小于最大线程数就创建一个新的线程
        {
            std::thread t([this]{this->Threadwork();});
            {
                std::unique_lock<std::mutex> lock(this->threadMutex_);
                threads_[t.get_id()] = std::move(t);
            }
            ++currentThreadnum_;
            ++idleThreadnum_;
            printf("resize ThreadPoll\n");
        }
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
            auto isTimeout = !this->condition_.wait_for(lock,std::chrono::seconds(this->timeOut_),[this]{
                return ((this->stop_==true) || (this->taskqueue_.empty()==false));
            });

            if((this->stop_ == true)&&(this->taskqueue_.empty()==true)) return; //停止且没任务

            if(isTimeout) //超时
            {
                if(this->currentThreadnum_.load()> this->minThreadnum_)
                {
                    printf("time out\n");
                    std::unique_lock<std::mutex> lock(deadThreadQueueMutex);
                    deadThreadQueue_.emplace(std::this_thread::get_id());
                    deadThreadQueueCondition_.notify_all();
                    --this->idleThreadnum_;
                    --this->currentThreadnum_;
                    return;
                }
                
            }else{
                task = move(this->taskqueue_.front());
                this->taskqueue_.pop();
            }
            --this->idleThreadnum_;
        }
        if(task) task();
        ++idleThreadnum_;
    }
}

void ThreadPoll::clearDeadThread()
{

    while(!stop_){
        std::unique_lock<std::mutex> lock(deadThreadQueueMutex);
        deadThreadQueueCondition_.wait(lock,[this]{return ((this->stop_==true) || (this->deadThreadQueue_.empty()==false));});
        while(!deadThreadQueue_.empty())
        {
            auto id = deadThreadQueue_.front();
            deadThreadQueue_.pop();
            {
                std::unique_lock<std::mutex> lock(this->threadMutex_);
                auto iter = threads_.find(id);
                assert(iter != threads_.end());
                assert(iter->second.joinable());
                iter->second.join();
                threads_.erase(iter);
            }
            
        }
    }

    std::unique_lock<std::mutex> lock(deadThreadQueueMutex);
    while(!deadThreadQueue_.empty())
    {
        auto id = deadThreadQueue_.front();
        deadThreadQueue_.pop();
        auto iter = threads_.find(id);
        {
            std::unique_lock<std::mutex> lock(this->threadMutex_);
            assert(iter != threads_.end());
            assert(iter->second.joinable());
            iter->second.join();
            threads_.erase(iter);
        }
        
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
    deadThreadQueueCondition_.notify_all();
    condition_.notify_all();
    printf("stop !\n");
    clearThread.join();
    for(auto &th : threads_)
    {
        th.second.join();
    }
}