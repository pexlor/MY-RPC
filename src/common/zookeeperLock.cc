#include "zookeeperLock.h"

ZookeeperLock::ZookeeperLock()
{
    std::string host = Config::GetGlobalConfig()->m_zookeeper_ip;
    std::string port = Config::GetGlobalConfig()->m_zookeeper_port;
    std::string connstr = host + ":" + port;
    m_zhandle = zookeeper_init(connstr.c_str(), nullptr, 30000, nullptr, nullptr, 0);//初始化
}

void ZookeeperLock::lock()
{
    //创建顺序临时节点
    //zoo_create(m_zhandle,"/lock/","0",1,&ZOO_OPEN_ACL_UNSAFE,);

    //获取所有子节点
    //zoo_get_children(m_zhandle,"/lock");
}

void ZookeeperLock::unlock()
{

}

void ZookeeperLock::WatchLock()
{

}