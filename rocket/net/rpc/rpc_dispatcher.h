#pragma once
#include "rocket/net/coder/abstract_protocol.h"
#include <map>
#include <memory>
#include <google/protobuf/service.h>

namespace rocket{
class RpcDispathcher{ //RPC分发器
public:
    typedef std::shared_ptr<google::protobuf::Service> service_ptr;
    void dispatch(AbstractProtocol::s_ptr request,AbstractProtocol::s_ptr response);
    void registerService(service_ptr service);

private:
    std::map<std::string,service_ptr> m_service_map;

    bool parseServiceFullName(const std::string& full_name,std::string& service_name,std::string& method_name);

};

}