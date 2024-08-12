#pragma once
#include <zookeeper/zookeeper.h>
#include <iostream>
#include <string>
#include <mutex>
#include <condition_variable>
#include <atomic>

class DistributedReadWriteLock{
public:
    DistributedReadWriteLock(const std::string &zookeeper_hosts, const std::string &lock_path);
    ~DistributedReadWriteLock();
    void lock_read();
    void unlock_read();
    void lock_write();
    void unlock_write();
private:
    std::string m_zookeeper_hosts;
    std::string m_lock_path;
    zhandle_t* m_zhandle;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic_bool isWriteLock;
    std::atomic_uint64_t readCount;
    void WatchLock();
};