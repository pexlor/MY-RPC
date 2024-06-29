#pragma once
#include <mutex>
#include <sys/syscall.h>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <vector>
#include <queue>

class ThreadPoll
{
private:
    std::vector<std::thread> threads_;//线程池中的线程
    std::queue<std::function<void()>> taskqueue_;//任务队列
    std::mutex mutex_;//互斥
    std::condition_variable condition_;//同步
    std::atomic_bool stop_;
    std::string threadtype_;  //io/works
public:
    ThreadPoll(size_t threadnum,const std::string &threadtype);
    ~ThreadPoll();
    void Stop();
    size_t size();
    void addtask(std::function<void()> task);
};




