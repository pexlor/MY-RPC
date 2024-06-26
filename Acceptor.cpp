#include "Acceptor.h"
/*EventLoop *loop_ = nullptr;
   Socket * servsock_ = nullptr;
   Channel * acceptchannel_ = nullptr;
   */
Acceptor::Acceptor(EventLoop* loop,const char * ip , uint16_t port):loop_(loop)
                    ,servsock_(createnonblocking())
                    ,acceptchannel_(loop_,servsock_.fd())
{
    
    InetAddress servaddr(ip,port);
    servsock_.setreuseaddr(true);
    servsock_.setreuseport(true);
    servsock_.settcpnodelay(true);
    servsock_.setkeepalive(true);
    servsock_.setipport(ip,port);
    servsock_.bind(servaddr);
    servsock_.listen(128);
    acceptchannel_.useet();
    acceptchannel_.setreadcallback(std::bind(&Acceptor::newconnction,this));
    acceptchannel_.enablereading();
}

void Acceptor::newconnction()
{
    InetAddress clientaddr;
    std::unique_ptr<Socket> clientsock(new Socket(servsock_.accept(clientaddr)));
    clientsock->setipport(clientaddr.ip(),clientaddr.port());
    newconnectioncb_(std::move(clientsock));
}

Acceptor::~Acceptor()
{
    
}

void Acceptor::setnewconnectioncb(std::function<void(std::unique_ptr<Socket>)> fn)
{
    newconnectioncb_ = fn;
}

