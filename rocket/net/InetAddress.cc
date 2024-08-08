#include "InetAddress.h"

InetAddress::InetAddress(const char *ip,uint16_t port)
{
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    inet_pton(AF_INET,ip,&addr_.sin_addr);
}

InetAddress::InetAddress(const sockaddr_in addr):addr_(addr)
{

}

InetAddress::InetAddress()
{

}
InetAddress::~InetAddress()
{

}

const char * InetAddress::ip() const
{
    return inet_ntoa(addr_.sin_addr);
}

uint16_t InetAddress:: port() const
{
    return ntohs(addr_.sin_port);
}

const sockaddr * InetAddress::addr() const
{
    return (sockaddr *)&addr_;
}

void InetAddress::setaddr(const sockaddr_in addr)
{
    addr_ = addr;
}

