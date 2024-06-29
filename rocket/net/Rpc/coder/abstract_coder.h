#ifndef ROCKET_NET_CODER_ABSTRACT_CODER_H
#define ROCKET_NET_CODER_ABSTRACT_CODER_H

#include <vector>

#include "rocket/net/Rpc/coder/abstract_coder.h"

namespace rocket {

class AbstractCoder {
public:

    // 将 message 对象转化为字节流，写入到 buffer
    virtual void encode(std::vector<AbstractProtocol::s_ptr>& messages, std::string out_buffer) = 0;

    // 将 buffer 里面的字节流转换为 message 对象
    virtual void decode(std::vector<AbstractProtocol::s_ptr>& out_messages, std::string buffer) = 0;

    virtual ~AbstractCoder() {}
};

};

#endif