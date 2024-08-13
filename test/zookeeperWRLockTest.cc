#include "../src/common/DistributedReadWriteLock.h"
#include <unistd.h>
#include <thread>
DistributedReadWriteLock lock("127.0.0.1:2181","iplock");

void testReadThread()
{
    for(int i=0;i<1000;i++){
        lock.lock_read();
        printf("lock read\n");
        sleep(1);
        lock.unlock_read();
    }
}

int main()
{
    std::thread thread1(testReadThread);
    std::thread thread2(testReadThread);
    std::thread thread3(testReadThread);

    thread1.join();
    thread2.join();
    thread3.join();
}