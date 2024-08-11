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
    DONE:  动态线程池,异步双缓冲日志,zookeeper,架构图

**遇到的问题：**

> 写日志的RAII锁问题（解决）：以RAII封装锁时，内部is_lock 变量没用原子类型，且在外部使用时显示调用了unlock()函数，线程竞争激烈时会出现释放锁问题（解锁并非原子过程）

> 连接丢失问题（解决）：Acceptor 中的acceptchannel_被设置为边沿触发模式。

> 快速反复连接问题（待解决）；目前思路：同时触发了接受和关闭

> 动态线程池回收问题（解决）：新增一个回收线程，把需要关闭的线程加入队列中。

zookeeper C API:
详见：[https://www.cnblogs.com/lidabo/p/14340535.html]

zookeeper 分布式锁：
>  最有意思的一集
实现依据：  
1、zookeeper中规定，在同一时刻，不能有多个客户端创建同一个节点，我们可以利用这个特性实现分布式锁。zookeeper临时节点只在session生命周期存在，session一结束会自动销毁。  

2、watcher机制，在代表锁资源的节点被删除，即可以触发watcher解除阻塞重新去获取锁，这也是zookeeper分布式锁较其他分布式锁方案的一大优势。  

实现方式：

1. 创建一个永久节点（/lock）
2. 首先由客户端连接到zookeeper，并创建一个临时顺序节点
3. 然后获取zookeeper中/lock下所有子节点，如果自己创建的节点为最小则获取到锁可以进行数据操作
4. 如果发现自己创建的节点并非lock所有子节点中最小的，说明自己还没有获取到锁，此时客户端需要找到比自己小的那个节点，同时对其注册事件监听器，监听删除事件。
5. 如果发现比自己小的那个节点被删除，则客户端的Watcher会收到相应通知，此时再次判断自己创建的节点是否是lock子节点中序号最小的，如果是则获取到了锁，如果不是则重复以上步骤继续获取到比自己小的一个节点并注册监听。
6. 在客户端中还要结合操作系统的锁实现

