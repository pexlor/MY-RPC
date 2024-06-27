#include "TcpServer.h"

TcpServer::TcpServer(const char * ip ,uint16_t port,int threadnum)
                    :threadnum_(threadnum),
                    mainloop_(new EventLoop(true,10,10)),
                    acceptor_(mainloop_,ip,port),
                    threadpoll_(threadnum_,"IO")
{
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
    acceptor_.setnewconnectioncb(std::bind(&TcpServer::newconnction,this,std::placeholders::_1));//绑定连接处理函数
    for(int i=0;i<threadnum_;i++)
    {
        subloops_.emplace_back(new EventLoop(false,3,6));//时间循环组
        subloops_[i]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));//绑定连接处理函数
        subloops_[i]->settimeoutcallback(std::bind(&TcpServer::removeconnection,this,std::placeholders::_1));
        threadpoll_.addtask(std::bind(&EventLoop::run,subloops_[i].get()));//加入线程池队列
    }
}

TcpServer::~TcpServer()
{
   
}

void TcpServer::start()
{
    mainloop_->run();//主线程运行
}

void TcpServer::stop()
{
    mainloop_->stop();
    for (int ii=0;ii<threadnum_;ii++)
    {
        subloops_[ii]->stop();
    }
}


void TcpServer::closeconnection(spConnection  conn)
{
    closeconnection_(conn);
    subloops_[conn->fd()%threadnum_]->closeconnection(conn->fd());
    
    {
        std::lock_guard<std::mutex> gd(mmutex);
        conns_.erase(conn->fd());
    }
    //delete conn;
}

void TcpServer::errorconnection(spConnection  conn)
{
    errorconnection_(conn);
    {
        std::lock_guard<std::mutex> gd(mmutex);
        conns_.erase(conn->fd());
    }
}

void TcpServer::newconnction(std::unique_ptr<Socket> clientsock) //accept会调用
{
    //printf("TcpServer: %d\n",clientsock->fd()%threadnum_);
    int sockfd = clientsock->fd();
    spConnection conn(new Connection(subloops_[sockfd%threadnum_].get(),std::move(clientsock)));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage,this,std::placeholders::_1,std::placeholders::_2));
    conn->setclosecallback(std::bind(&TcpServer::closeconnection,this,std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection,this,std::placeholders::_1));
    conn->setcomplatecallback(std::bind(&TcpServer::sendcomplate,this,std::placeholders::_1));

    {
        std::lock_guard<std::mutex> gd(mmutex);
        conns_[conn->fd()] = conn;
    }

    subloops_[conn->fd()%threadnum_]->newconnection(conn);
    newconnction_(conn);
}

void TcpServer::onmessage(spConnection  conn,std::string &message)
{
    //printf("Connection2 recv: %s \n",message.data());
    onmessage_(conn,message);
}

void TcpServer::sendcomplate(spConnection  conn)
{
    //printf("send complate\n");
    sendcomplate_(conn);
}


void TcpServer::epolltimeout(EventLoop *loop)
{
    //printf("epoll time out\n");
    epolltimeout_(loop);
}

void TcpServer::setnewconnction(std::function<void(spConnection )> fn)\
{
    newconnction_ = fn;
}

void TcpServer::setcloseconnection(std::function<void(spConnection )> fn)
{
    
    closeconnection_ = fn;
    
}

void TcpServer::seterrorconnection(std::function<void(spConnection )> fn)
{
    errorconnection_ = fn;
    
}

void TcpServer::setonmessage(std::function<void(spConnection ,std::string &)> fn)
{
    onmessage_ = fn;
    
}

void TcpServer::setsendcomplate(std::function<void(spConnection )> fn)
{
    sendcomplate_ = fn;  
}

void TcpServer::setepolltimeout(std::function<void(EventLoop *)> fn)
{
    epolltimeout_ = fn;
}

void TcpServer::removeconnection(int fd)
{
    {
        std::lock_guard<std::mutex> gd(mmutex);
        conns_.erase(fd);
    }
}