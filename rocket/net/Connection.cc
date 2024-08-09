#include "Connection.h"

Connection::Connection(EventLoop* loop,std::unique_ptr<Socket> clientsock)
    :loop_(loop),
    clientsock_(std::move(clientsock)),
    disconnect_(false),
    clientchannel_(new Channel(loop_,clientsock_->fd())) 
{
    clientchannel_->useet();
    clientchannel_->setreadcallback(std::bind(&Connection::onmessagecallback,this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));
    clientchannel_->setwritecallback(std::bind(&Connection::writecallback,this));
    clientchannel_->enablereading();
}

Connection::~Connection()
{
    std::cout << "connection over\n";
}

int Connection::fd() const
{
    return clientsock_->fd();
}

std::string Connection::ip() const
{
    return clientsock_->ip();
}

uint16_t Connection::port() const
{
    return clientsock_->port();
}

void Connection::closecallback()
{
    disconnect_ = true;
    clientchannel_->remove(); // 可能是这有问题
    closecallback_(shared_from_this());
}

void Connection::errorcallback()
{
    disconnect_ = true;
    clientchannel_->remove();
    errorcallback_(shared_from_this());
}

void Connection::setclosecallback(std::function<void(spConnection)> fn)
{
    closecallback_ = fn;
}

void Connection::seterrorcallback(std::function<void(spConnection)> fn)
{
    errorcallback_ = fn;
}

void Connection::setonmessagecallback(std::function<void(spConnection ,std::string &)> fn)
{
    onmessagecallback_ = fn;
}
void Connection::setcomplatecallback(std::function<void(spConnection)> fn)
{
    complatecallback_ = fn;
}

void Connection::onmessagecallback()
{
    char buffer[1024];
    while(true)
    {
        bzero(buffer,sizeof(buffer));
        ssize_t nread = recv(fd(),buffer,sizeof(buffer)-1,0);
        //printf("rece nread %d\n",nread);
        if(nread > 0)
        {
            inputbuffer_.append(buffer,nread);
            
        }else if(nread == -1 && errno == EINTR) 
        {
            continue;
        }else if(nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            while(true)
            {
                std::string message;
                if(inputbuffer_.pickmessage(message)==false) break;
                lastatime_ = Timestamp::now();
                if(onmessagecallback_ == nullptr)
                {
                    printf("onmessagecallback_ error in fd: %d\n",this->fd());
                    //exit(-1);
                }
                if(shared_from_this() == nullptr)
                {
                    printf("shared_from_this error in fd: %d\n",this->fd());
                }
                onmessagecallback_(shared_from_this(),message); 
            }
            break;
        }else if(nread == 0)
        {
            closecallback();
            break;
        }
    }
}

void Connection::sendinloop( std::shared_ptr<std::string> data ,size_t size)
{
    outputbuffer_.appendwithsep(data.get()->c_str(),size);//放入发送缓冲区
    clientchannel_->enablewriting();
}

void Connection::send(const char * data ,size_t size)
{
    if(disconnect_ == true){
        return;
    }
    std::shared_ptr<std::string> temp(new std::string);
    temp->assign(data,size);
    if(loop_->isinloopthread())//是否为io线程
    {
        sendinloop(temp,size);
    }else
    {
        //交给IO线程执行
        loop_->queueinloop(std::bind(&Connection::sendinloop,this,temp,size));
    }
}

void Connection::writecallback()
{
    int writen = ::send(fd(),outputbuffer_.data(),outputbuffer_.size(),0);
    
    if(writen > 0)
    {
        outputbuffer_.erase(0,writen);
    }
    if(outputbuffer_.size()==0)
    {
        
        clientchannel_->disablewriting();
        complatecallback_(shared_from_this());
    }
}

bool Connection::timeout(time_t now,int val)
{
    return now - lastatime_.toint() > val;
}

