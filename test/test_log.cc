#include "../src/common/log.h"
#include "../src/common/config.h"

void * fun(void*)
{
    int i = 10000;
    while(i--)
    {
        DEBUGLOG("test log");
    }
    return NULL;
}

int main()
{
    Config::InitConfig("/root/Downloads/MY-RPC/conf/rocket.xml");
    Logger::Init();
    pthread_t thread1;
    pthread_create(&thread1,NULL,&fun,NULL);
    pthread_t thread2;
    pthread_create(&thread2,NULL,&fun,NULL);
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    return 0;
}