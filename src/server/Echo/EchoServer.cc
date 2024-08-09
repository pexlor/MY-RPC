#include "EchoServer.h"

EchoServer::EchoServer(const char *ip ,const uint16_t port,int threadnum,int workthreadnum)
                        :tcpserver_(ip,port,threadnum),threadpoll_(workthreadnum,"work")
{
    //TCPserver的函数接口
    tcpserver_.setnewconnction(std::bind(&EchoServer::HandleNewConnction,this,std::placeholders::_1));
    tcpserver_.seterrorconnection(std::bind(&EchoServer::HandleErrorConnection,this,std::placeholders::_1));
    tcpserver_.setcloseconnection(std::bind(&EchoServer::HandleCloseConnection,this,std::placeholders::_1));
    tcpserver_.setonmessage(std::bind(&EchoServer::HandleMessage,this,std::placeholders::_1,std::placeholders::_2));
    tcpserver_.setsendcomplate(std::bind(&EchoServer::HandleSendComplate,this,std::placeholders::_1));
    tcpserver_.setepolltimeout(std::bind(&EchoServer::HandleEpollTimeout,this,std::placeholders::_1));
}

EchoServer::~EchoServer()
{
    
}

void EchoServer::Start()
{
    tcpserver_.start();
}


void EchoServer::Stop()
{
    //停止工作线程
    threadpoll_.Stop();
    //停止
    tcpserver_.stop();
}

void EchoServer::HandleNewConnction(spConnection  coon)
{
    std::cout << "New connection" << std::endl;
}

void EchoServer::HandleCloseConnection(spConnection  conn )
{
    std::cout << "Close connection" << std::endl;
}

void EchoServer::HandleErrorConnection(spConnection  conn)
{
    std::cout << "Error connection" << std::endl;
}

void EchoServer::HandleMessage(spConnection conn,std::string &message)//业务处理的代码
{
   // std::cout << message <<std::endl;
    if(threadpoll_.size()==0) //没有工作线程的情况
    {
        OnMessage(conn,message);//直接调用处理函数
    }else
    {
        //把处理任务放入线程池任务队列中
        threadpoll_.addtask(std::bind(&EchoServer::OnMessage,this,conn,message));
    }
}

char head_buf[] = "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Type: text/html;charset=UTF-8\r\nContent-Length: 1033\r\nConnection: close\r\n\r\n";
char Phead_buf[] = "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Type: image/jpg;charset=UTF-8\r\nContent-Length: 35825\r\nConnection: close\r\n\r\n";

int a = 0;
void EchoServer::OnMessage(spConnection conn,std::string &message)
{
    message="reply:"+message;          // 回显业务。
    conn->send(message.data(),message.size());
}

void EchoServer::HandleSendComplate(spConnection conn)
{
    
}

void EchoServer::HandleEpollTimeout(EventLoop *loop)
{
    //std::cout << "Time out" << std::endl;
}   