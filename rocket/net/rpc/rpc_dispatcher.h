#pragma once
#include "rocket/net/coder/abstract_protocol.h"
#include <map>
#include <memory>
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include "rocket/net/coder/tinypb_protocol.h"
#include "rocket/common/log.h"
#include "rocket/common/error_code.h"
#include "rocket/net/rpc/rpc_controller.h"
#include "rocket/net/tcp/tcp_connection.h"
namespace rocket{
class TcpConnection;

class RpcDispathcher{ //RPC分发器
public:
    typedef std::shared_ptr<google::protobuf::Service> service_ptr;
    void dispatch(AbstractProtocol::s_ptr request,AbstractProtocol::s_ptr response,TcpConnection* connection);
    void registerService(service_ptr service);
private:
    std::map<std::string,service_ptr> m_service_map;
    void setTinyPBError(std::shared_ptr<TinyPBProtocol> msg,int32_t err_code,const std::string err_info);
    bool parseServiceFullName(const std::string& full_name,std::string& service_name,std::string& method_name);

};

}