#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include "ThreadPoll.h"
#include <signal.h>
#include <string>
#include <regex>    // 正则表达式
#include <unordered_map>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
class Http
{
private:
    /* data */
    enum PARSE_STATE{
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH, 
    };
    enum STATUS_CODE
    {
        SUCCESS = 200,
        ERROR = 404,
    };
    std::string RequstMethod_;
    std::string RequstUrl_;
    std::string RequstHttpEdition_;
    const std::string FileRootPath_ = "/http";
    std::string FilePath_;
    std::string FileType_;
    int FileSize_;
    std::string Message_;
    std::string MessageCopy_;
    std::unordered_map<std::string,std::string> RequestHead_;
    PARSE_STATE state_ = REQUEST_LINE;
    char * FileAddress;
    bool ParseRequestLine(const std::string & line); //解析请求行
    bool ParseHeader(const std::string & line); //解析请求头
    void ParseBody(const std::string & line); //请求解析体
    void ParsePath();
    bool Parse();
    std::string GetLine();
public:
    Http(std::string &Message);
    ~Http();
    std::string Respond();
};

