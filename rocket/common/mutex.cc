#include "mutex.h"

namespace rocket{

template<class T>
ScopeMutex<T>::ScopeMutex(T& mutex)
{
    this->m_mutex = mutex;
    m_mutex.lock();
    m_is_lock = true;
}
template<class T>
ScopeMutex<T>::~ScopeMutex()
{
    m_is_lock = false;
    m_mutex.unlock();
}
template<class T>
void ScopeMutex<T>::lock()
{
    if (!m_is_lock) 
    {
        m_mutex.lock();
        m_is_lock = true;
    }
}
template<class T>
void ScopeMutex<T>::unlock()
{
    if (m_is_lock) {
        m_is_lock = false;
        m_mutex.unlock();
    }
}



Mutex::Mutex()
{
    pthread_mutex_init(&m_mutex, NULL);
}
Mutex::~Mutex()
{
    pthread_mutex_destroy(&m_mutex);
}
void  Mutex::lock()
{
    pthread_mutex_lock(&m_mutex);
}
void  Mutex::unlock()
{
    pthread_mutex_unlock(&m_mutex);
}
pthread_mutex_t*  Mutex::getMutex()
{
    return &m_mutex;
}