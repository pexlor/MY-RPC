#pragma once
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <atomic>
#include <functional>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include "Buffer.h"

//todo: 线程准备好后才能append
class AsyncLogger {
private:
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
    typedef BufferVector::value_type BufferPtr;

    std::unique_ptr<std::thread> thread_;
    std::mutex mutex_;
    std::condition_variable cond_;

    BufferPtr currentBuf_;
    BufferPtr nextBuf_;
    BufferVector buffers_;

    const int flushInterval_;//刷新时间
    const std::string basename_;

    std::mutex runMutex_;
    std::condition_variable runCond_;
    std::atomic<bool> isRuning_;

    int fileFd_;
    const uint64_t maxBuffSize = 50000;

private:
    void AsyncLoggerLoop();

public:
    void append(const std::string & logline,int len);
    void start();
    void stop();
    ~AsyncLogger();
    AsyncLogger(const std::string & basename,int flushInterval = 3);

};