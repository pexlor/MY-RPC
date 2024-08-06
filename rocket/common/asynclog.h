#pragma once
#include <string>
#include <mutex>
class AsyncLog {
private:
    std::mutex bufMutex;
    Buffer currentBuf;
    


public:
    void append(const std::string * logline,int len);


};