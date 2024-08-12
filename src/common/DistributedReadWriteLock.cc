#include "DistributedReadWriteLock.h"
/*
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
};*/
DistributedReadWriteLock::DistributedReadWriteLock(const std::string &zookeeper_hosts, const std::string &lock_path)
    :m_zookeeper_hosts(zookeeper_hosts),
     m_lock_path(lock_path)
{
    m_zhandle = zookeeper_init(m_zookeeper_hosts.c_str(), nullptr, 30000, nullptr, nullptr, 0);//初始化
    if(m_zhandle == nullptr){
        perror("Failed to connect to ZooKeeper");
        exit(-1);
    }
}

DistributedReadWriteLock::~DistributedReadWriteLock()
{
    zookeeper_close(m_zhandle);
}

void DistributedReadWriteLock::lock_read()
{
    //创建顺序临时节点
    //zoo_create(m_zhandle,"/lock/","0",1,&ZOO_OPEN_ACL_UNSAFE,);

    //获取所有子节点
    //zoo_get_children(m_zhandle,"/lock");
}

void DistributedReadWriteLock::unlock_read()
{
    
}

void DistributedReadWriteLock::lock_write()
{
    //创建顺序临时节点
    //zoo_create(m_zhandle,"/lock/","0",1,&ZOO_OPEN_ACL_UNSAFE,);

    //获取所有子节点
    //zoo_get_children(m_zhandle,"/lock");
}

void DistributedReadWriteLock::unlock_write()
{

}

void DistributedReadWriteLock::WatchLock()
{

}