
#pragma once

#include <memory>
#include <google/protobuf/service.h>
#include "rocket/net/PxLib/InetAddress.h"
#include "rocket/net/PxLib/EventLoop.h"
#include "rocket/net/PxLib/Acceptor.h"

#define NEWMESSAGE(type, var_name) \
    std::shared_ptr<type> var_name = std::make_shared<type>(); \

#define NEWRPCCONTROLLER(var_name) \
    std::shared_ptr<rocket::RpcController> var_name = std::make_shared<rocket::RpcController>(); \

#define NEWRPCCHANNEL(addr, var_name) \
    std::shared_ptr<rocket::RpcChannel> var_name = std::make_shared<rocket::RpcChannel>(std::make_shared<rocket::IPNetAddr> (addr)); \

#define CALLRPC(addr, stub_name, method_name, controller, request, response, closure) \
    { \
        NEWRPCCHANNEL(addr, channel); \
        channel->Init(controller, request, response, closure); \
        stub_name(channel.get()).method_name(controller.get(), request.get(), response.get(), closure.get()); \
    } \ 

class RpcChannel : public google::protobuf::RpcChannel, public std::enable_shared_from_this<RpcChannel> {

public:

    using s_ptr = std::shared_ptr<RpcChannel>;
    using controller_s_ptr = std::shared_ptr<google::protobuf::RpcController>;
    using message_s_ptr = std::shared_ptr<google::protobuf::Message>;
    using closure_s_ptr = std::shared_ptr<google::protobuf::Closure>;

    RpcChannel(const char * ip ,uint16_t port);

    ~RpcChannel();

    void Init(controller_s_ptr controller, message_s_ptr req, message_s_ptr rsp, closure_s_ptr done);
    
    void CallMethod(const google::protobuf::MethodDescriptor* method, 
                            google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                            google::protobuf::Message* response, google::protobuf::Closure* done);

    google::protobuf::RpcController* GetController();

    google::protobuf::Message* GetRequest();

    google::protobuf::Message* GetResponse();

    google::protobuf::Closure* GetClosure();

    //TcpClient* GetTcpClient();

    //TimerEvent::s_ptr GetTimerEvnet();

private:
    InetAddress m_peer_addr ;
    InetAddress m_local_addr ;

    controller_s_ptr m_controller {nullptr};
    message_s_ptr m_request {nullptr};
    message_s_ptr m_response {nullptr};
    closure_s_ptr m_closure {nullptr};

    bool m_is_init {false};

    //std::unique_ptr<EventLoop> m_loop;
    //Acceptor acceptor_;//接收线程

    Socket servsock_;
    //Channel acceptchannel_;

   // TcpClient::s_ptr m_client;

   // TimerEvent::s_ptr m_timer_event;

};


