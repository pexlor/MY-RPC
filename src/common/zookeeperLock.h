#pragma once
#include <string>
#include <zookeeper/zookeeper.h>
#include "config.h"
class ZookeeperLock{
public:
    ZookeeperLock();
    ~ZookeeperLock();
    void lock();
    void unlock();
private:
    std::string m_ip;
    std::string m_port;
    zhandle_t* m_zhandle;
    void WatchLock();
};