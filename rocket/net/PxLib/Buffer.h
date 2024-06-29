#pragma once
#include <string>
#include <iostream>
#include <cstring>

class Buffer
{
private:
    std::string buf_;
    const uint16_t sep_; //报文分割符
public:
    Buffer(uint16_t sep = 0);
    ~Buffer();

    void append(const char * data ,size_t size);
    size_t size();
    const char *data();
    void clear();
    void erase(size_t pos,size_t nn);
    void appendwithsep(const char * data , size_t size);
    bool pickmessage(std::string &ss);
};