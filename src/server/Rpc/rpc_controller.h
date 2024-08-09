#pragma once
#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>

class RpcController : public google::protobuf::RpcController{
public:
    RpcController(){}
    ~RpcController(){}
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void StartCancel();
    void SetFailed(const std::string& reason);
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* callback);

    void SetError(int32_t error_code,const std::string error_info);
    int32_t GetErrorCode();
    std::string GetErrorInfo();
    void SetMsgId(const std::string& req_id);
    std::string GetMsgId();
    void SetTimeout(int timeout);
    int GetTimeout();
     
private:
    int32_t m_error_code{0};
    std::string m_error_info;
    std::string m_req_id;
    bool m_is_failed = false;
    bool m_is_cancled = false;

    int m_timeout = 1000;
};

