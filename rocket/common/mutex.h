#pragma once
#include <pthread.h>

namespace rocket{

template<class T>
class ScopeMutex {
public:
    ScopeMutex(T& mutex);
    ~ScopeMutex();
    void lock();
    void unlock();
private:
    T& m_mutex;
    bool m_is_lock = false;
};

class Mutex {

public:
    Mutex();
    ~Mutex();
    void lock();
    void unlock();
    pthread_mutex_t* getMutex();
private:
    pthread_mutex_t m_mutex;

};


}