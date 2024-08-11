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
#include "src/common/log.h"
#include "src/common/config.h"
#include "src/server/Rpc/coder/abstract_protocol.h"
#include "src/server/Rpc/coder/tinypb_coder.h"
#include "src/server/Rpc/coder/tinypb_protocol.h"
#include "src/server/Rpc/rpc_channel.h"
#include "src/server/Rpc/rpc_closure.h"
#include "src/server/Rpc/rpc_controller.h"
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
    control.SetMsgId("9912312998888");

    std::shared_ptr<RpcClosure> closure = std::make_shared<RpcClosure>(nullptr,[=]{return true;});
    control.SetTimeout(10000);
    mychannel.Init(NULL, NULL, NULL, NULL);
    
    for(int i=0;i<1;i++)
    {
        Order_Stub(&mychannel).makeOrder(&control, &requst, &reponse, closure.get());
        std::cout << "Getresponse:"<<reponse.order_id() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    
    Config::InitConfig("/root/Downloads/MY-RPC/conf/RPConfig.xml");
    Logger::Init();
    // 获取开始时间点
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "start id taken: " <<  argv[1] << std::endl;
    testRpcChannel();
    // 获取结束时间点
    auto end = std::chrono::high_resolution_clock::now();
    // 计算持续时间
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // 输出持续时间
    std::cout << "Total time taken: " << duration.count() << " milliseconds" <<" id :" << argv[1] << std::endl;
} 

