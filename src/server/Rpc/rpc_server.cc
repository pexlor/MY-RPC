#include "rpc_server.h"

RPCServer::RPCServer(const char *ip ,const uint16_t port,int threadnum,int workthreadnum)
            :tcpserver_(ip,port == 0 ? Config::GetGlobalConfig()->m_port:port,threadnum == 0 ? Config::GetGlobalConfig()->m_io_threads :threadnum),
            threadpoll_(workthreadnum == 0 ? Config::GetGlobalConfig()->m_work_threads:workthreadnum,"work")
{
    //TCPserver的函数接口
    tcpserver_.setnewconnction(std::bind(&RPCServer::HandleNewConnction,this,std::placeholders::_1));
    tcpserver_.seterrorconnection(std::bind(&RPCServer::HandleErrorConnection,this,std::placeholders::_1));
    tcpserver_.setcloseconnection(std::bind(&RPCServer::HandleCloseConnection,this,std::placeholders::_1));
    tcpserver_.setonmessage(std::bind(&RPCServer::HandleMessage,this,std::placeholders::_1,std::placeholders::_2));
    tcpserver_.setsendcomplate(std::bind(&RPCServer::HandleSendComplate,this,std::placeholders::_1));
    tcpserver_.setepolltimeout(std::bind(&RPCServer::HandleEpollTimeout,this,std::placeholders::_1));

    m_coder = new TinyPBCoder();
}

RPCServer::~RPCServer()
{
    
}

void RPCServer::Start()
{
    ZkClient zkCli;
    std::string ip = Config::GetGlobalConfig()->m_ip;
    uint16_t port = Config::GetGlobalConfig()->m_port;
    
    zkCli.Start();
    //std::cout<<RpcDispatcher::GetRpcDispatcher()->m_service_map.size()<<std::endl;
    for(auto it : RpcDispatcher::GetRpcDispatcher()->m_service_map){
        std::string service_name = it.first;
        for(int i = 0 ;i < it.second.get()->GetDescriptor()->method_count();i++){
            std::string method_name = it.second.get()->GetDescriptor()->method(i)->name();
            std::cout<< service_name << "." << method_name <<std::endl;
            std::string service_path = "/" + service_name;
            zkCli.Create(service_path.c_str(), nullptr, 0);
            std::string method_path = service_path + "/" + method_name;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            std::cout<< method_path_data << std::endl;
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }
    tcpserver_.start();
    DEBUGLOG("RPCServer Start Successed!");
}


void RPCServer::Stop()
{
    //停止工作线程
    threadpoll_.Stop();
    //停止
    tcpserver_.stop();
    DEBUGLOG("RPCServer Stop!");
}

void RPCServer::HandleNewConnction(spConnection  coon)
{
    //std::cout << "New connection" << std::endl;
}

void RPCServer::HandleCloseConnection(spConnection  conn )
{
    //std::cout << "Close connection" << std::endl;
}

void RPCServer::HandleErrorConnection(spConnection  conn)
{
    //std::cout << "Error connection" << std::endl;
}

void RPCServer::HandleMessage(spConnection conn,std::string &message)//业务处理的代码
{
   // std::cout << message <<std::endl;
    if(threadpoll_.size()==0) //没有工作线程的情况
    {
        OnMessage(conn,message);//直接调用处理函数
    }else
    {
        //把处理任务放入线程池任务队列中
        threadpoll_.addtask(std::bind(&RPCServer::OnMessage,this,conn,message));
    }
}

void RPCServer::OnMessage(spConnection conn,std::string &message)
{
    std::vector<AbstractProtocol::s_ptr> result;
    std::vector<AbstractProtocol::s_ptr> reply_messages;
    m_coder->decode(result, message);//解码
    for (size_t i = 0; i < result.size(); i++) {
        // 针对每一个请求，调用 Rpc 方法，获取响应 message
        // 将响应 message 放入到发送缓冲区，监听可写事件回包
        //INFOLOG("successfully get request[%s] from client [%s]", result[i]->m_msg_id.c_str(), m_peer_addr->toString().c_str());
        std::shared_ptr<TinyPBProtocol> message = std::make_shared<TinyPBProtocol>();
        RpcDispatcher::GetRpcDispatcher()->dispatch(result[i], message); //处理请求
        reply_messages.push_back(message);
    }
    std::string m_out_buffer;
    m_coder->encode(reply_messages, m_out_buffer);
    conn->send(m_out_buffer.c_str(),m_out_buffer.size());
}

void RPCServer::HandleSendComplate(spConnection conn)
{
    
}

void RPCServer::HandleEpollTimeout(EventLoop *loop)
{
    //std::cout << "Time out" << std::endl;
}

void RPCServer::RegisterRpcService(std::shared_ptr<google::protobuf::Service> service)
{
    RpcDispatcher::GetRpcDispatcher()->registerService(service); //注册服务
}