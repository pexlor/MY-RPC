#include <iostream>
#include <queue>
#include <functional>
#include <string>
#include <thread>
#include <unistd.h>
std::queue<std::function<void()>> m_queue;

void printf111(const char * data){
    std::cout << data << std::endl;
}

void Test()
{
    while(true)
    {
        while(!m_queue.empty())
        {
            auto it = m_queue.front();
            m_queue.pop();
            while(1){
                it();
                sleep(1);
            }
            
        }
    }
}

void onMessage(const char * data){
    m_queue.push([data](){ printf111(data); });
}

int main()
{
    char *a=NULL;
    std::thread t(Test);
    if(1)
    {
        std::string s = "asda";
        a = (char *)s.c_str();
        onMessage(s.c_str());
    }
    
    printf("aaa:%s\n",a);
    t.join();
    return 0;
}