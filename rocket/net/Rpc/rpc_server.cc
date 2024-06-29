#include "rpc_server.h"

/*class EchoServer
{
private:
    TcpServer tcpserver_;
public:
    EchoServer(const char * ip ,uint16_t port);
    ~EchoServer();
    void Start();
    void HandleNewConnction(Socket* clientsock);
    void HandleCloseConnection(Connection * conn );
    void HandleErrorConnection(Connection * conn);
    void HandleMessage(Connection * conn,string);
    void HandleSendComplate(Connection * conn);
    void HandleEpollTimeout(EventLoop *loop);
};*/

RPCServer::RPCServer(const char *ip ,const uint16_t port,int threadnum,int workthreadnum)
                        :tcpserver_(ip,port,threadnum),threadpoll_(workthreadnum,"work")
{
    //TCPserver的函数接口
    tcpserver_.setnewconnction(std::bind(&RPCServer::HandleNewConnction,this,std::placeholders::_1));
    tcpserver_.seterrorconnection(std::bind(&RPCServer::HandleErrorConnection,this,std::placeholders::_1));
    tcpserver_.setcloseconnection(std::bind(&RPCServer::HandleCloseConnection,this,std::placeholders::_1));
    tcpserver_.setonmessage(std::bind(&RPCServer::HandleMessage,this,std::placeholders::_1,std::placeholders::_2));
    tcpserver_.setsendcomplate(std::bind(&RPCServer::HandleSendComplate,this,std::placeholders::_1));
    tcpserver_.setepolltimeout(std::bind(&RPCServer::HandleEpollTimeout,this,std::placeholders::_1));
}

RPCServer::~RPCServer()
{
    
}

void RPCServer::Start()
{
    tcpserver_.start();
}


void RPCServer::Stop()
{
    //停止工作线程
    threadpoll_.Stop();
    //停止
    tcpserver_.stop();
}

void RPCServer::HandleNewConnction(spConnection  coon)
{
    std::cout << "New connection" << std::endl;
}

void RPCServer::HandleCloseConnection(spConnection  conn )
{
    std::cout << "Close connection" << std::endl;
}

void RPCServer::HandleErrorConnection(spConnection  conn)
{
    std::cout << "Error connection" << std::endl;
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
    std::cout << message; 
    std::vector<rocket::AbstractProtocol::s_ptr> result;
    std::vector<rocket::AbstractProtocol::s_ptr> reply_messages;
    m_coder->decode(result, message);
    for (size_t i = 0; i < result.size(); i++) {
        // 针对每一个请求，调用 Rpc 方法，获取响应 message
        // 将响应 message 放入到发送缓冲区，监听可写事件回包
        //INFOLOG("successfully get request[%s] from client [%s]", result[i]->m_msg_id.c_str(), m_peer_addr->toString().c_str());
        std::shared_ptr<rocket::TinyPBProtocol> message = std::make_shared<rocket::TinyPBProtocol>();
        // message->m_pb_data = "hello. this is rocket rpc test data";
        // message->m_msg_id = result[i]->m_msg_id;
        rocket::RpcDispatcher::GetRpcDispatcher()->dispatch(result[i], message, NULL); 
        reply_messages.push_back(message);
    }
    std::string m_out_buffer;
    m_coder->encode(reply_messages, m_out_buffer);
    // 监听写回调函数
    //listenWrite();
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
    rocket::RpcDispatcher::GetRpcDispatcher()->registerService(service); //注册服务
}