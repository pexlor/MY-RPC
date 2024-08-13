#pragma once
#include <zookeeper/zookeeper.h>
#include <iostream>
#include <string>
#include <cstring>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <algorithm>
#include <semaphore.h>
class DistributedReadWriteLock{
public:
    DistributedReadWriteLock(const std::string &zookeeper_hosts, const std::string &lock_path);
    ~DistributedReadWriteLock();
    void lock_read();
    void unlock_read();
    void lock_write();
    void unlock_write();
public:
    std::mutex m_read_mutex;
    std::condition_variable m_read_cv;

    std::mutex m_write_mutex;
    std::condition_variable m_write_cv;

    std::atomic_bool min_write_node_deleted {false};
    std::atomic_bool min_read_node_deleted {false};
    std::string min_write_node_path;
    std::string min_read_node_path;
private:
    std::string m_zookeeper_hosts;
    std::string m_lock_name;
    zhandle_t* m_zhandle;
    std::atomic_bool isWriteLock {false};
    std::atomic_uint64_t readCount;

    std::string read_lock_node_name;
    std::string write_lock_node_name;
    void WatchLock();
};