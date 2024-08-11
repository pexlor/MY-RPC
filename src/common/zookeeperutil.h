#pragma once

#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <zookeeper/zookeeper.jute.h>
#include <string>
#include <iostream>
#include "config.h"
//封装zk客户端
class ZkClient {
    public:
        ZkClient();
        ~ZkClient();
        void Start();
        void Create(const char* path, const char* data, int datalen, int state = 0);
        std::string GetData(const char* path);
    private:
        zhandle_t* m_zhandle;
};