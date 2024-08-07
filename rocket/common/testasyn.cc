#include "asynclog.h"

AsyncLogger as("asdasd");

void testFun(int id)
{
    as.start();
    std::string a = "1111111111111123123213asdasdasd12312312asfrewfhdsiuahfdshjkfbskdjkjajkdfjqgdhjg1y8329817389172893\n";
    while(1){
        as.append(&a,a.size());
    }
}

int main()
{
    testFun(1);
}