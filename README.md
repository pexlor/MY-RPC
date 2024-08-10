使用的技术：
Liunx + protobuf + tinyxml + c++

实现的功能:

1. 基础网络框架（Reactor架构）
2. 配置功能
3. 动态线程池
4. 双缓冲异步日志/同步日志
5. 自定义rpc协议的编码解码
6. rpc分发器，rpc服务端，rpc客户端

网络服务器框架（multiple reactor + threadpool）：
![Network Server Framework](/res/NetworkServerFramework.png "Network Server Framework")

RPC自定义协议协议：
![TinyPB](/res/TinyPB.png "TingPB")

性能测试：
测试环境： 系统(centos 8.0 64bit), 硬件(2核｜2GIB｜系统盘40GIB｜峰值带宽 2Mbit/s)
测试指标：（每个连接发生10w条消息）(与sogo开源srpc对比)  
    单个连接QPS：me:1.1w  srpc:1.6w  差距45%  
    20个连接QPS：me:2.8w  srpc:3.6w  差距28%（注：srpc的CPU占用率仅为35%）  
    总结：性能差距较大  
    
后续的更新：
    TODO：异步双缓冲日志 动态线程池 使用io_uring 协程 zookeeper 架构图 性能优化
    DONE:  动态线程池,异步双缓冲日志

**遇到的问题：**

> 写日志的RAII锁问题（解决）：以RAII封装锁时，内部is_lock 变量没用原子类型，且在外部使用时显示调用了unlock()函数，线程竞争激烈时会出现释放锁问题（解锁并非原子过程）

> 连接丢失问题（解决）：Acceptor 中的acceptchannel_被设置为边沿触发模式。

> 快速反复连接问题（待解决）；目前思路：同时触发了接受和关闭

> 动态线程池回收问题（解决）：新增一个回收线程，把需要关闭的线程加入队列中。
