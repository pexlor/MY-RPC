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
#include "src/common/log.h"
#include "src/common/config.h"
#include "order.pb.h"
#include "src/server/Rpc/rpc_server.h"

class OrderImpl : public Order {
    
public:
    void makeOrder(google::protobuf::RpcController* controller,
                       const ::makeOrderRequest* request,
                       ::makeOrderResponse* response,
                       ::google::protobuf::Closure* done) {
        
        if (request->price() < 10) {
            response->set_ret_code(-1);
            response->set_res_info("short balance");
            return;
        }
        response->set_order_id("20230829");
    }
};

int main(int argc , char * argv[])
{
    Config::InitConfig("/root/Downloads/MY-RPC/conf/RPConfig.xml");
    Logger::Init();
    std::shared_ptr<OrderImpl> service = std::make_shared<OrderImpl>();
    RPCServer rpcServer("127.0.0.1");
    rpcServer.RegisterRpcService(service);
    rpcServer.Start();
    return 0;
}