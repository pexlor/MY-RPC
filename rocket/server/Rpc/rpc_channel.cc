#include "rpc_channel.h"
#include "coder/abstract_coder.h"
#include "coder/tinypb_coder.h"

RpcChannel::RpcChannel(const char * ip ,uint16_t port):
    m_ip(ip),
    m_port(port)
{
    m_coder_= new TinyPBCoder();
}

RpcChannel::~RpcChannel() {
    DEBUGLOG("~RpcChannel");
}

void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method, 
                        google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                        google::protobuf::Message* response, google::protobuf::Closure* done) {

    std::shared_ptr<TinyPBProtocol> req_protocol = std::make_shared<TinyPBProtocol>();
    RpcController* my_controller = dynamic_cast<RpcController*>(controller);
    if (my_controller == NULL) {
        ERRORLOG("failed Callmethod, RpcController convert error");
        return;
    }

    if (my_controller->GetMsgId().empty()) {
        req_protocol->m_msg_id = MsgIDUtil::GenMsgID();
        my_controller->SetMsgId(req_protocol->m_msg_id);
    } else {
        req_protocol->m_msg_id = my_controller->GetMsgId();
    }

    req_protocol->m_method_name = method->full_name();
    INFOLOG("%s | call method name [%s]", req_protocol->m_msg_id.c_str(), req_protocol->m_method_name.c_str());

    if (!m_is_init) {
        std::string err_info = "RpcChannel not init";
        my_controller->SetError(ERROR_RPC_CHANNEL_INIT, err_info);
        ERRORLOG("%s | %s, RpcChannel not init [%s]", req_protocol->m_msg_id.c_str(), err_info.c_str(), 
                request->ShortDebugString().c_str());
        return;
    }

    if (!request->SerializeToString(&(req_protocol->m_pb_data))) {
        std::string err_info = "failed to serialize";
        my_controller->SetError(ERROR_FAILED_SERIALIZE, err_info);
        ERRORLOG("%s | %s, origin request [%s]", req_protocol->m_msg_id.c_str(), err_info.c_str(), 
                request->ShortDebugString().c_str());
        return;
    }

    s_ptr channel = shared_from_this();
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_port);
    if (inet_pton(AF_INET, m_ip.c_str(), &(server_addr.sin_addr)) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        close(sockfd);
        return;
    }

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        close(sockfd);
        return;
    }

    std::vector<AbstractProtocol::s_ptr> messages;
    messages.push_back(req_protocol);
    printf("messages size: %d\n",messages.size());

    std::string out_buf;
    m_coder_->encode(messages,out_buf);
    printf("encode ok %d\n",out_buf.size());

    send(sockfd,out_buf.c_str(),out_buf.size(),0);
    printf("send ok\n");
    char buf[1024] = {0};

    recv(sockfd,buf,1024,0);
    printf("recv ok %s\n",buf);
    std::vector<AbstractProtocol::s_ptr> out_messages;

    std::string out_buf2(buf);
    m_coder_->decode(out_messages,out_buf2);
    if(out_messages.size() != 1)
    {
        return;
    }
    if(!response->ParseFromString(std::dynamic_pointer_cast<TinyPBProtocol>(out_messages[0])->m_pb_data))//反序列化
    {
        return;
    }
    close(sockfd);
}

void RpcChannel::Init(controller_s_ptr controller, message_s_ptr req, message_s_ptr rsp, closure_s_ptr done) {
    if (m_is_init) {
        return;
    }
    m_controller = controller;
    m_request = req;
    m_response = rsp;
    m_closure = done;
    m_is_init = true;
}

google::protobuf::RpcController* RpcChannel::GetController() {
    return m_controller.get(); 
}

google::protobuf::Message* RpcChannel::GetRequest() {
    return m_request.get();
}

google::protobuf::Message* RpcChannel::GetResponse() {
    return m_response.get(); 
}

google::protobuf::Closure* RpcChannel::GetClosure() {
    return m_closure.get(); 
}




