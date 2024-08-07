#include "Buffer.h"

/*
class Buffer
{
private:
    string buf_;
public:
    Buffer();
    ~Buffer();

    void append(const char * data ,size_t size);
    size_t size();
    const char *data();
    void clear();
};
*/
#define MAX_BUF_SIZE 1000000

Buffer::Buffer(uint16_t sep):sep_(sep),maxLen_(MAX_BUF_SIZE),bufSize(0)
{

}

Buffer::Buffer(uint64_t maxLen):sep_(0),maxLen_(maxLen),bufSize(0)
{
    if(maxLen > MAX_BUF_SIZE)
    {
        maxLen = MAX_BUF_SIZE;
    }
    maxLen_ = maxLen;
    //buf_.resize(maxLen_);
}

void Buffer::reset()
{
    buf_ = "";
    //buf_.resize(maxLen_);
}

Buffer::~Buffer()
{

}

uint64_t Buffer::avail()
{
    return maxLen_-buf_.size();
}

void Buffer::append(const char * data ,size_t size)
{
    if(avail() >= size){
        buf_.append(data,size);
    }
}

size_t Buffer::size()
{
    return buf_.size();
}

const char *Buffer::data()
{
    return buf_.data();
}

void Buffer::clear()
{
    buf_.clear();
}

void Buffer::erase(size_t pos,size_t nn)
{
    buf_.erase(pos,nn);
}

void Buffer::appendwithsep(const char * data , size_t size)
{
    if(sep_== 0)
    {
        buf_.append(data,size);
    }else if(sep_ == 1)
    {
        buf_.append((char *)&size,4);
        buf_.append(data,size);
    }
    
}

bool Buffer::pickmessage(std::string &ss)
{
    if(buf_.size()==0)
    {
        return false;
    }
    if(sep_== 0)
    {
        ss = buf_;
        buf_.clear();
    }else if(sep_ == 1)
    {
        int len;
        memcpy(&len,buf_.data(),4);
        if(buf_.size() < len + 4)
        {
            return false;
        }
        ss = buf_.substr(4,len);
        buf_.erase(0,len+4);
    }

    return true;
}