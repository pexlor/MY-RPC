#include "rocket/net/Rpc/rpc_dispatcher.h"



static RpcDispatcher* g_rpc_dispatcher = NULL;


//设置错误信息
void RpcDispatcher::setTinyPBError(std::shared_ptr<TinyPBProtocol> msg,int32_t err_code,const std::string err_info)
{
    msg->m_err_code = err_code;
    msg->m_err_info = err_info;
    msg->m_err_info_len = err_info.length();
}

//处理RPC请求
void RpcDispatcher::dispatch(AbstractProtocol::s_ptr request,AbstractProtocol::s_ptr response,TcpConnection* connection)
{
    std::shared_ptr<TinyPBProtocol> req_protocol = std::dynamic_pointer_cast<TinyPBProtocol>(request);
    std::shared_ptr<TinyPBProtocol> rsp_protocol = std::dynamic_pointer_cast<TinyPBProtocol>(response);

    std::string method_full_name = req_protocol->m_method_name;
    std::string service_name;
    std::string method_name;
    if(!parseServiceFullName(method_full_name,service_name,method_name))
    {
        setTinyPBError(rsp_protocol,ERROR_PARSE_SERVICE_NAME,"parse sevice name error");
        return;
    }

    auto it = m_service_map.find(service_name);//找到对应
    if(it == m_service_map.end())
    {
        setTinyPBError(rsp_protocol,ERROR_SERVICE_NOT_FOUND,"parse sevice not found");
        return;
    }

    service_ptr service =(*it).second;
    const google::protobuf::MethodDescriptor * method = service->GetDescriptor()->FindMethodByName(method_name);
    if(method == NULL)
    {
        setTinyPBError(rsp_protocol,ERROR_METHOD_NOT_FOUND,"parse method not found");
        return;
    }
    google::protobuf::Message* req_msg = service->GetRequestPrototype(method).New();//获得请求消息原型
    DEBUGLOG("start parse from string");
    //反序列
    if(!req_msg->ParseFromString(req_protocol->m_pb_data))
    {
        setTinyPBError(rsp_protocol,ERROR_FAILED_DESERIALIZE,"parse sevice failed deserialize");
        if (req_msg != NULL) {
            delete req_msg;
            req_msg = NULL;
        }
        return;
    }

    DEBUGLOG("Get rpc request[%s]",req_msg->ShortDebugString().c_str());

    DEBUGLOG("start parse from string");
    google::protobuf::Message* rsp_msg = service->GetResponsePrototype(method).New();

    RpcController rpcController;
    //IPNetAddr::s_ptr local_addr = std::make_shared<IPNetAddr>("127.0.0.1",1234);
    //rpcController.SetLocalAddr(connection->getLocalAddr());
    //rpcController.SetPeerAddr(connection->getPeerAddr());
    rpcController.SetReqId(req_protocol->m_msg_id);

    DEBUGLOG("start call method");
    service->CallMethod(method,&rpcController,req_msg,rsp_msg,NULL);
    DEBUGLOG("end call method");
    
    rsp_protocol->m_method_name = req_protocol->m_method_name;
    rsp_protocol->m_msg_id = req_protocol->m_msg_id;

    

    if(!rsp_msg->SerializeToString(&(rsp_protocol->m_pb_data)))
    {
        setTinyPBError(rsp_protocol,ERROR_FAILED_SERIALIZE,"parse sevice failed serialize");
        if (rsp_msg != NULL) {
            delete rsp_msg;
            rsp_msg = NULL;
        }
        return;
    }

    rsp_protocol->m_err_code = 0;
    DEBUGLOG("%s | dispatch success, request[%s], response[%s]", 
        req_protocol->m_msg_id.c_str(), req_msg->ShortDebugString().c_str(), rsp_msg->ShortDebugString().c_str());
    if (req_msg != NULL) {
        delete req_msg;
        req_msg = NULL;
    }
    if (rsp_msg != NULL) {
        delete rsp_msg;
        rsp_msg = NULL;
    }
}

//注册服务
void RpcDispatcher::registerService(service_ptr service)
{
    std::string service_name = service->GetDescriptor()->full_name();
    m_service_map[service_name] = service;
}

//解析rpc服务名 xxx.xxx
bool RpcDispatcher::parseServiceFullName(const std::string& full_name,std::string& service_name,std::string& method_name)
{
    if(full_name.empty())
    {
        ERRORLOG("full name empty");
        return false;
    }
    size_t i = full_name.find_first_of(".");
    if(i == full_name.npos)
    {
        ERRORLOG("not find . in full name %s",full_name.c_str());
        return false;
    }
    service_name = full_name.substr(0,i);
    method_name = full_name.substr(i+1,full_name.length()-i-1);
    INFOLOG("pasrse service_name[%s] and method_name[%s]",service_name.c_str(),method_name.c_str());
    return true;
}

RpcDispatcher* RpcDispatcher::GetRpcDispatcher()
{
    if (g_rpc_dispatcher == NULL) {
        g_rpc_dispatcher = new RpcDispatcher();
    }
    return g_rpc_dispatcher;
}

