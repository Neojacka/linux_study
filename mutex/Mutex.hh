#pragma once
#include <iostream>
#include <pthread.h>

namespace hzz
{
    class Mutex
    {
    public:
        Mutex(pthread_mutex_t *lock)
            : _lock(lock)
        {
        }
        void lock()
        {
            if (_lock)
                pthread_mutex_lock(_lock);
        }
        void unlock()
        {
            if (_lock)
                pthread_mutex_unlock(_lock);
        }
        ~Mutex()
        {
        }

    private:
        pthread_mutex_t *_lock; // 设置为指针是因为lock，unlock等都是&pthread_mutex_t
    };
    class lock_guard
    {
    public:
        lock_guard(pthread_mutex_t* lock)
            :_mutex(lock)
        {
            _mutex.lock();
        }
        ~lock_guard()
        {
            _mutex.unlock();
        }
    private:
        Mutex _mutex;
    };
}