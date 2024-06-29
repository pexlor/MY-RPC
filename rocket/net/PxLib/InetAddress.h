#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>


/*
    基础地址类，封装了对地址的操作
*/
class InetAddress
{ 
private:
    sockaddr_in addr_;
public:
    InetAddress();
    InetAddress(const char *ip,uint16_t port);
    InetAddress(const sockaddr_in addr);
    
    ~InetAddress();
    const char *ip() const;
    uint16_t port() const;
    const sockaddr *addr() const;
    void setaddr(const sockaddr_in addr);
};