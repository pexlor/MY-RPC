
#pragma once

#include "abstract_protocol.h"
#include <string>
namespace rocket
{

//自定义RPC协议类

struct TinyPBProtocol : public AbstractProtocol{

public:
    static char PB_START; //开始符
    static char PB_END; //结束符

    int32_t m_pk_len = 0;//整包长度
    int32_t m_msg_id_len = 0;//msg id长度
    //req_id父类已有
    int32_t m_method_name_len = 0; //方法名长度
    std::string m_method_name; //方法名
    int32_t m_err_code = 0; //错误码
    int32_t  m_err_info_len = 0; //错误信息长度
    std::string m_err_info;//错误信息
    std::string m_pb_data; //序列化数据
    int32_t m_check_sum = 0;//校验和
    bool parse_success = false;
};

char TinyPBProtocol::PB_START = 0x02;
char TinyPBProtocol::PB_END = 0x03;

} // namespace rocket
