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
#include <atomic>
#include <cassert>
#include <unordered_map>
class ThreadPoll
{
private:
    std::mutex deadThreadQueueMutex;
    std::condition_variable deadThreadQueueCondition_;//条件变量同步
    std::queue<std::thread::id> deadThreadQueue_;

    std::mutex threadMutex_;
    std::unordered_map<std::thread::id,std::thread> threads_;//线程池中的线程

    std::mutex mutex_;//互斥
    std::condition_variable condition_;//条件变量同步
    std::queue<std::function<void()>> taskqueue_;//任务队列

    std::thread clearThread;//清理线程，清理多余的超时线程
    std::atomic_bool stop_;
    std::string threadtype_;  //io/works
    size_t timeOut_; //超时时间，线程长时间无响应则清理线程
    size_t maxThreadnum_; //允许的最大总线程数
    size_t minThreadnum_; //允许的最小总线程数
    std::atomic<size_t>  currentThreadnum_;//目前拥有总线程数
    std::atomic<size_t>  idleThreadnum_; //目前空闲的线程数

private:
    void Threadwork();
    void clearDeadThread();
    void joinDeadThreadQueue();
public:
    ThreadPoll(size_t minthreadnum,const std::string &threadtype,size_t maxThreadnum = 0,size_t timeOut = 180);

    // 使用 std::enable_if 禁用复制构造函数和移动构造函数
    ThreadPoll(ThreadPoll&&) = delete;
    ThreadPoll &operator=(const ThreadPoll &) = delete;

    ~ThreadPoll();
    void Stop();
    size_t size();
    void addtask(std::function<void()> task);
};




