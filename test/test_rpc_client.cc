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
#include "rocket/server/Rpc/coder/abstract_protocol.h"
#include "rocket/server/Rpc/coder/tinypb_coder.h"
#include "rocket/server/Rpc/coder/tinypb_protocol.h"
#include "rocket/server/Rpc/rpc_channel.h"
#include "rocket/server/Rpc/rpc_closure.h"
#include "rocket/server/Rpc/rpc_controller.h"
#include <google/protobuf/service.h>
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


void testRpcChannel()
{
    RpcChannel mychannel("127.0.0.1",5000);
    makeOrderRequest requst;
    makeOrderResponse reponse;
    requst.set_price(100);
    requst.set_goods("apple");
    RpcController control;
    control.SetMsgId("99998888");

    std::shared_ptr<RpcClosure> closure = std::make_shared<RpcClosure>(nullptr,[=]{return true;});
    control.SetTimeout(10000);
    mychannel.Init(NULL, NULL, NULL, NULL); 
    Order_Stub(&mychannel).makeOrder(&control, &requst, &reponse, closure.get());

}

int main(int argc, char *argv[])
{
    
    Config::SetGlobalConfig("/home/pexlor/Downloads/rpc/conf/rocket.xml");
    Logger::Init();
    testRpcChannel();
} 

