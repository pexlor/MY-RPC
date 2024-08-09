#include "ThreadPoll.h"
#include <iostream>
class ThreadPoll;
ThreadPoll threadpoll(2,"io",4,5);

void test_function() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Task executed by thread " << std::this_thread::get_id() << std::endl;

}

void Test()
{
    int n=3;
    while(n--){
        threadpoll.addtask(test_function);
    }
}

int main()
{
    std::thread thread1(Test);
    std::thread thread2(Test);
    std::thread thread3(Test);
    std::thread thread4(Test);
    sleep(10);
    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
    threadpoll.Stop();
}