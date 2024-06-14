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
    pthread_t thread;
    pthread_create(&thread,NULL,&fun,NULL);
    int i = 20;
    while(i--)
    {
        DEBUGLOG("test log");
    }
    pthread_join(thread,NULL);
    return 0;
}