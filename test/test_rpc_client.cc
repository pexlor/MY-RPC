#include <assert.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <memory>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include "rocket/common/log.h"
#include "rocket/common/config.h"
//#include "rocket/net/coder/string_coder.h"
#include "rocket/net/Rpc/coder/abstract_protocol.h"
#include "rocket/net/Rpc/coder/tinypb_coder.h"
#include "rocket/net/Rpc/coder/tinypb_protocol.h"
//#include "rocket/net/rpc/rpc_channel.h"
#include "rocket/net/Rpc/rpc_closure.h"
#include "rocket/net/Rpc/rpc_controller.h"
#include "order.pb.h"

// 网络通讯的客户端程序。
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage:./client ip port\n"); 
        printf("example:./client 192.168.150.128 5085\n\n"); 
        return -1;
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char buf[1024];
 
    if ((sockfd=socket(AF_INET,SOCK_STREAM,0))<0) { printf("socket() failed.\n"); return -1; }
    
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) != 0)
    {
        printf("connect(%s:%s) failed.\n",argv[1],argv[2]); close(sockfd);  return -1;
    }

    std::shared_ptr<rocket::TinyPBProtocol> message = std::make_shared<rocket::TinyPBProtocol>();
    // message->info = "hello rocket!";
    message->m_msg_id = "99998888";
    message->m_pb_data = "test pb data";
    makeOrderRequest request;
    request.set_price(100);
    request.set_goods("apple");
    if (!request.SerializeToString(&(message->m_pb_data))) {
        ERRORLOG("serialize error");
        return;
    }
    message->m_method_name = "Order.makeOrder";

    client.writeMessage(message, [request](rocket::AbstractProtocol::s_ptr msg_ptr) {
        DEBUGLOG("send message success request[%s]", request.ShortDebugString().c_str());
    });

    std::string msg = "99998888";
    client.readMessage(msg, [](rocket::AbstractProtocol::s_ptr msg_ptr) {
        // DEBUGLOG("send message success");
        std::shared_ptr<rocket::TinyPBProtocol> message = std::dynamic_pointer_cast<rocket::TinyPBProtocol>(msg_ptr);
        DEBUGLOG("msg_id [%s], get response %s", message->m_msg_id.c_str(), message->m_pb_data.c_str());

        makeOrderResponse response;
        if (!response.ParseFromString(message->m_pb_data)) {
            ERRORLOG("deserialize error");
            return;
        }

        DEBUGLOG("get response success, response[%s]", response.ShortDebugString().c_str());

    });

    

    /*
    for (int ii=0;ii<10;ii++)
    {
        memset(buf,0,sizeof(buf));
        sprintf(buf,"这是第%d个超级女生。",ii);

        send(sockfd,buf,strlen(buf),0);  // 把请求报文发送给服务端。
        
        memset(buf,0,sizeof(buf));
        recv(sockfd,buf,1024,0);           // 读取报文内容。

        printf("recv:%s\n",buf);
sleep(1);
    }
    */


} 

