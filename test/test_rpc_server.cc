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
#include <google/protobuf/service.h>
#include "rocket/common/log.h"
#include "rocket/common/config.h"
#include "order.pb.h"
#include "rocket/net/Rpc/rpc_server.h"
class OrderImpl : public Order {
    
public:
    void makeOrder(google::protobuf::RpcController* controller,
                       const ::makeOrderRequest* request,
                       ::makeOrderResponse* response,
                       ::google::protobuf::Closure* done) {

        //APPDEBUGLOG("start sleep 5s");
        //sleep(5);
        //APPDEBUGLOG("end sleep 5s");
        if (request->price() < 10) {
            response->set_ret_code(-1);
            response->set_res_info("short balance");
            return;
        }
        response->set_order_id("20230829");
        //APPDEBUGLOG("makeOrder success.");
    }
};

int main(int argc , char * argv[])
{
    if(argc != 3)
    {
        printf("usgae :./tecepoll ip port\n");
        return -1;
    }
    Config::SetGlobalConfig("/home/pexlor/Downloads/reactor/conf/rocket.xml");
    Logger::SetGetGloballLogger();
    std::shared_ptr<OrderImpl> service = std::make_shared<OrderImpl>();
    RPCServer rpcServer(argv[1],atoi(argv[2]),3,3);
    rpcServer.RegisterRpcService(service);
    
    rpcServer.Start();
    return 0;
}