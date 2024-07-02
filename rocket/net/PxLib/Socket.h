#pragma once
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "InetAddress.h"

int createnonblocking();

/*封装socket类，对socket的一些操作进行封装，如listen，bind*/
class Socket
{
private:
    const int fd_;
    std::string ip_;
    uint16_t port_;
public:
    Socket(int fd);
    ~Socket();

    
    void setreuseaddr(bool on);
    void setreuseport(bool on);
    void settcpnodelay(bool on);
    void setkeepalive(bool on);
    
    void setipport(std::string ip,uint16_t port);

    int fd() const;
    std::string ip() const;
    uint16_t port() const;

    void bind(const InetAddress & servaddr);
    void listen(int nn);
    int accept(InetAddress & clientaddr);
    int connect(InetAddress& clientaddr);
};