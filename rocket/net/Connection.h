#pragma once
#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"
#include "Timestamp.h"
#include <functional>
#include <memory>
#include <atomic>
#include <sys/syscall.h>
class Connection;
class Channel;
using spConnection = std::shared_ptr<Connection>;

class EventLoop;

class Connection : public std::enable_shared_from_this<Connection>
{
private:
    EventLoop* loop_;
    std::unique_ptr<Socket> clientsock_;

    std::unique_ptr<Channel> clientchannel_;

    Buffer inputbuffer_;
    Buffer outputbuffer_;
    std::atomic_bool disconnect_;

    Timestamp lastatime_;
    
    std::function<void(spConnection )> closecallback_;
    std::function<void(spConnection )> errorcallback_;
    std::function<void(spConnection )> complatecallback_;
    std::function<void(spConnection ,std::string &)> onmessagecallback_;

public:
    Connection(EventLoop* loop,std::unique_ptr<Socket> clientsock);
    ~Connection();
    
    int fd() const;
    std::string ip() const;
    uint16_t port() const;
    void send(const char * data ,size_t size);
    void sendinloop(const char * data ,size_t size);
    void close();

    void onmessagecallback();
    void closecallback();
    void errorcallback();
    void writecallback();

    void setclosecallback(std::function<void(spConnection )> fn);
    void seterrorcallback(std::function<void(spConnection )> fn);
    void setcomplatecallback(std::function<void(spConnection )> fn);
    void setonmessagecallback(std::function<void(spConnection ,std::string &)> fn);
    
    bool timeout(time_t now,int val);
};


