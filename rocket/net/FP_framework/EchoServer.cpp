#include "EchoServer.h"

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
    std::cout << message; 
    Http http(message);
    std::string re = http.Respond();
    //std::cout <<"re = " << http.Size_ <<std::endl;
    conn->send(re.c_str(),re.size());
    // if(a==0)
    // {
    //     conn->send(head_buf,sizeof(head_buf));
    //     int filefd = ::open("res/demo.html",O_RDONLY);
    //     struct stat file_st;
    //     fstat(filefd,&file_st);
    // // std::cout << file_st.st_size <<std::endl;
    //     char *buf = (char *)malloc(sizeof(char) * file_st.st_size);
    //     read(filefd,buf,file_st.st_size);
    //     conn->send(buf,file_st.st_size);
    //     close(filefd);
    //     a = 1;
    // }else if(a == 1)
    // {
    //     conn->send(Phead_buf,sizeof(head_buf));
    //     struct stat file_st;
    //     int filefd = ::open("res/10001.jpg",O_RDONLY);
    //     fstat(filefd,&file_st);
    //    // std::cout << file_st.st_size <<std::endl;
    //     char *buf = (char *)malloc(sizeof(char) * file_st.st_size);
    //     read(filefd,buf,file_st.st_size);
    //     printf("%s\n",buf);
    //     conn->send(buf,file_st.st_size);
    //     close(filefd);
    //     a = 2;
    // }else if(a == 2)
    // {
    //     a = 0;
    // }
}

void EchoServer::HandleSendComplate(spConnection conn)
{
    
}

void EchoServer::HandleEpollTimeout(EventLoop *loop)
{
    //std::cout << "Time out" << std::endl;
}   