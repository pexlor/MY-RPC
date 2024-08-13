#include "DistributedReadWriteLock.h"

void watcher(zhandle_t *zzh, int type, int state, const char *path, void *watcherCtx) {
    // 处理事件，例如节点创建、删除等
    DistributedReadWriteLock *self = (DistributedReadWriteLock *) watcher;
    if(path == self->min_write_node_path)
    {
        if(type == ZOO_DELETED_EVENT) { //node deleted, so awake   
            self->min_write_node_deleted = true;
            self->m_read_cv.notify_all();
        }
    }
    if(path == self->min_read_node_path){
        if(type == ZOO_DELETED_EVENT) { //node deleted, so awake   
            self->min_read_node_deleted = true;
            self->m_write_cv.notify_all();
        }
    }
}

DistributedReadWriteLock::DistributedReadWriteLock(const std::string &zookeeper_hosts, const std::string &lock_name)
    :m_zookeeper_hosts(zookeeper_hosts),
     m_lock_name("/"+lock_name)
{
    m_zhandle = zookeeper_init(m_zookeeper_hosts.c_str(), watcher, 30000, nullptr, nullptr, 0);//初始化
    if(m_zhandle == nullptr){
        perror("Failed to connect to ZooKeeper\n");
        exit(-1);
    }
}

DistributedReadWriteLock::~DistributedReadWriteLock()
{
    zookeeper_close(m_zhandle);
}

void DistributedReadWriteLock::unlock_write()
{
    auto path = m_lock_name + "/" + write_lock_node_name;
    auto ret = zoo_delete(m_zhandle, path.c_str() , -1);
    isWriteLock = false;
    if(ret != ZOK){
        printf("unlock_write failed\n");
        exit(-1);
    }
}

void DistributedReadWriteLock::lock_write()
{
    //检查远程是否加写锁
    //创建临时顺序节点
    std::string lock_path = m_lock_name + "/" + "write_";
    std::cout << "lock_path = "+lock_path <<std::endl;
    char node_path_buffer[512]; int buffer_len = 512;
    auto ret = zoo_create(m_zhandle,lock_path.c_str(),nullptr,0,&ZOO_OPEN_ACL_UNSAFE,ZOO_EPHEMERAL | ZOO_SEQUENCE,node_path_buffer,buffer_len);
    if(!(ret == ZOK)){
        printf("ceate /lock node faild\n");
        exit(-1);
    }
    std::string self_node_path(node_path_buffer);
    std::string self_node_name  = self_node_path.substr(m_lock_name.size(),self_node_path.size());
       
    do{
        //获取所有子节点
        struct String_vector ChildrenList; 
        ret = zoo_get_children(m_zhandle,m_lock_name.c_str(),0,&ChildrenList);
        if(ret !=ZOK || ChildrenList.count <= 0){
            printf("get children node faild\n");
            exit(-1);
        }
        std::vector<std::string> all_children;
        for(int i = 0; i < ChildrenList.count; i++) {
            all_children.emplace_back(ChildrenList.data[i]);
            delete ChildrenList.data[i];
        }
        std::sort(all_children.begin(), all_children.end());
        if(all_children[0] == self_node_name){
            isWriteLock = true;
            break;
        }
        isWriteLock = true;
        std::string  min_node_path;
        for(int i = 0;i<all_children.size();i++){
            if(all_children[i] == self_node_name){
                min_node_path = m_lock_name + "/" + all_children[i-1];
                break;
            }
        }
        struct Stat stat;
        int len = 256;
        ret = zoo_get(m_zhandle,min_node_path.c_str(),1,node_path_buffer,&len,&stat);
        if(ret  == ZNONODE){
            continue;
        }
        if(!(ret != ZOK)){
            printf("set watch falied\n");
            exit(-1);
        }
        std::unique_lock<std::mutex> lock(m_write_mutex);
        min_read_node_path = min_node_path;
        m_write_cv.wait(lock,[this](){return min_read_node_deleted == true;});
    }while(true);
    write_lock_node_name = self_node_name;
}

void DistributedReadWriteLock::lock_read()
{
    //检查远程是否加写锁
    //创建临时顺序节点
    std::string lock_path = m_lock_name + "/" + "read_";
    std::cout << "lock_path = "+lock_path <<std::endl;
    char node_path_buffer[512]; int buffer_len = 512;
    auto ret = zoo_create(m_zhandle,lock_path.c_str(),nullptr,0,&ZOO_OPEN_ACL_UNSAFE,ZOO_EPHEMERAL | ZOO_SEQUENCE,node_path_buffer,buffer_len);
    if(!(ret == ZOK)){
        printf("ceate /lock node faild\n");
        exit(-1);
    }
    std::string self_node_path(node_path_buffer);
    std::string self_node_name  = self_node_path.substr(m_lock_name.size(),self_node_path.size());
       
    do{
        //获取所有子节点
        struct String_vector ChildrenList; 
        ret = zoo_get_children(m_zhandle,m_lock_name.c_str(),0,&ChildrenList);
        if(ret !=ZOK || ChildrenList.count <= 0){
            printf("get children node faild\n");
            exit(-1);
        }
        std::vector<std::string> all_children;
        for(int i = 0; i < ChildrenList.count; i++) {
            all_children.emplace_back(ChildrenList.data[i]);
            delete ChildrenList.data[i];
        }
        std::sort(all_children.begin(), all_children.end());
        if(all_children[0] == self_node_name){
            break;
        }
        std::string  min_node_path;
        for(int i = 0;i<all_children.size();i++){
            if(all_children[i] == self_node_name){
                break;
            }
            if(all_children[i].find("write_")){
                min_node_path = m_lock_name + "/" + all_children[i];
                isWriteLock = true;
                break;
            }
        }
        if(!isWriteLock){
            break;
        }
        struct Stat stat;
        int len = 256;
        ret = zoo_get(m_zhandle,min_node_path.c_str(),1,node_path_buffer,&len,&stat);
        if(ret  == ZNONODE){
            continue;
        }
        if(!(ret != ZOK)){
            printf("set watch falied\n");
            exit(-1);
        }
        std::unique_lock<std::mutex> lock(m_read_mutex);
        min_write_node_path = min_node_path;
        m_read_cv.wait(lock,[this](){return min_write_node_deleted == true;});
    }while(true);
    read_lock_node_name = self_node_name;
}

void DistributedReadWriteLock::unlock_read()
{
    auto path = m_lock_name + "/" +read_lock_node_name;
    auto ret = zoo_delete(m_zhandle, path.c_str() , -1);
    if(ret != ZOK){
        printf("unlock_read failed\n");
        exit(-1);
    }
}