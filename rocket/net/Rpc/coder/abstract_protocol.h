#pragma once

#include <memory>
#include <string>


struct AbstractProtocol : public std::enable_shared_from_this<AbstractProtocol>{

public:
    using s_ptr = std::shared_ptr<AbstractProtocol>;

    virtual ~AbstractProtocol() {}

public:
    std::string m_msg_id;   // 请求号，唯一标识一个请求或响应

};

