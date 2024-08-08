#include "Socket.h"

/*
class Socket
{
private:
    const int fd_;
public:
    Socket(int fd);
    ~Socket();

    int fd() const;
    void setreuseaddr(bool on);
    void setreuseport(bool on);
    void settcpnodelay(bool on);
    void setkeepalive(bool on);
    void bind(const InetAddress & servaddr);
    void listen(int nn = 128);
    void accept(const InetAddress & clientaddr);
};*/

int createnonblocking()
{
    int listenfd = socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK,IPPROTO_TCP);
    if(listenfd < 0)
    {
        printf("creat socket error!\n");
        exit(-1);
    }
    return listenfd;
}

Socket::Socket(int fd):fd_(fd)
{

}
Socket::~Socket()
{
    close(fd_);
}

int Socket::fd() const
{
    //std::cout << fd_ <<std::endl;
    return this->fd_;
}

std::string Socket::ip() const
{
    return ip_;
}

uint16_t Socket::port() const
{
    return port_;
}

void Socket::setreuseaddr(bool on)
{
    int opt = on ? 1 : 0;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&opt,static_cast<socklen_t>(sizeof opt));
}
void Socket::setreuseport(bool on)
{
    int opt = on ? 1 : 0;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEPORT,&opt,static_cast<socklen_t>(sizeof opt));
}
void Socket::settcpnodelay(bool on)
{
    int opt = on ? 1 : 0;
    setsockopt(fd_,SOL_SOCKET,TCP_NODELAY,&opt,static_cast<socklen_t>(sizeof opt));
}
void Socket::setkeepalive(bool on)
{
    int opt = on ? 1 : 0;
    setsockopt(fd_,SOL_SOCKET,SO_KEEPALIVE,&opt,static_cast<socklen_t>(sizeof opt));
}

void Socket::setipport(std::string ip,uint16_t port)
{
    ip_ = ip;
    port_ = port;
}
void Socket::bind(const InetAddress & servaddr)
{
    if(::bind(fd_,servaddr.addr() ,sizeof(sockaddr)) < 0)
    {
        perror("bind failed");
        close(fd_);
        exit(-1);
    }
    setipport(servaddr.ip(),servaddr.port());
}

void Socket::listen(int nn = 128)
{
    if(::listen(fd_,nn) != 0)
    {
        perror("lsiten failed");
        close(fd_);
        exit(-1);
    }
}

int Socket::accept(InetAddress& clientaddr)
{
    struct sockaddr_in peeraddr;
    socklen_t len = sizeof(peeraddr);
    int clientfd = accept4(fd_,(struct sockaddr *)&peeraddr,&len,SOCK_NONBLOCK);
    clientaddr.setaddr(peeraddr);
    return clientfd;
}

int Socket::connect(InetAddress& clientaddr)
{
    if(::connect(fd_, (struct sockaddr *)clientaddr.addr(),sizeof(struct sockaddr_in))!=-1)
    {
        return 1;
    }
    return -1;

}