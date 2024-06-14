#include "../rocket/common/log.h"
#include "../rocket/common/config.h"

void * fun(void*)
{
    int i = 20;
    while(i--)
    {
        DEBUGLOG("test log");
    }
    return NULL;
}

int main()
{
    rocket::Config::SetGlobalConfig("/home/pexlor/Downloads/rpc/conf/rocket.xml");
    rocket::Logger::SetGetGloballLogger();
    pthread_t thread1;
    pthread_create(&thread1,NULL,&fun,NULL);
    pthread_t thread2;
    pthread_create(&thread2,NULL,&fun,NULL);
    int i = 20;
    while(i--)
    {
        DEBUGLOG("test log");
    }
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    return 0;
}