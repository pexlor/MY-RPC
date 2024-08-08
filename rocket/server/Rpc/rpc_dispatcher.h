#pragma once
#include "coder/abstract_protocol.h"
#include <map>
#include <memory>
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include "coder/tinypb_protocol.h"
#include "rocket/common/log.h"
#include "rocket/common/error_code.h"
#include "rpc_controller.h"

/**
 * @param RPC分发器,负责把请求分发给合适的处理函数处理
*/

class RpcDispatcher{ //RPC分发器
public:
    static RpcDispatcher* GetRpcDispatcher();
    using service_ptr=std::shared_ptr<google::protobuf::Service> ;
    void dispatch(AbstractProtocol::s_ptr request,AbstractProtocol::s_ptr response);
    void registerService(service_ptr service);
    void unregisterService(service_ptr service);
private:
    std::map<std::string,service_ptr> m_service_map;
    void setTinyPBError(std::shared_ptr<TinyPBProtocol> msg,int32_t err_code,const std::string err_info);
    bool parseServiceFullName(const std::string& full_name,std::string& service_name,std::string& method_name);
};

