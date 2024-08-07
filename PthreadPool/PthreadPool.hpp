#pragma once
#include "pthread.hh"
#include "Mutex.hh"
#include <vector>
#include <queue>
#include <string>
#include "task.hh"
using namespace std;
using namespace hzz;

int _cap = 5; // 线程池线程个数
template <class T>
class PthreadPool;
template<class T>
class PthreadData
{
public:
    PthreadData(PthreadPool<T>* pool,string& name)
        :_pool(pool),pthread_name(name)
        {}
public:
    PthreadPool<T>* _pool;
    string pthread_name;
};

template <class T>
class PthreadPool {
private:
    static void *taskfun(void *arg)
    {
        PthreadData<T> *data = static_cast<PthreadData<T> *>(arg);
        while (true)
        {
            data->_pool->lock();
            // 判断是否有任务
            while (data->_pool->empty())
            {
                data->_pool->wait();
            }
            T t;
            data->_pool->pop(&t);
            data->_pool->unlock();
            cout << "线程" << data->pthread_name<<"完成任务：" << t() << endl;
        }
        delete data;
        return nullptr;
    }

public:
    void lock() { pthread_mutex_lock(&_mutex); }
    void unlock() { pthread_mutex_unlock(&_mutex); }
    void wait() { pthread_cond_wait(&_cond, &_mutex); }
    bool empty() { return _task_queue.empty(); }
    void pop(T* out) 
    {
        lock_guard lock(&_mutex);
        *out = _task_queue.front();
        _task_queue.pop();
    }

    PthreadPool(int cap = _cap) : _num(cap) 
    {
        pthread_cond_init(&_cond, nullptr);
        pthread_mutex_init(&_mutex, nullptr);
        // 创建一批线程
        for (int i = 0; i < cap; i++) 
        {
            _pthread.push_back(new pthread());
        }
    }

    void run() 
    {
        for (auto& p : _pthread) 
        {   
            PthreadData<T>* data=new PthreadData<T>(this,p->getname());
            p->start(taskfun, data);
        }
    }

    // 放任务
    void push(const T& in) 
    {
        lock_guard lock(&_mutex);
        _task_queue.push(in);
        pthread_cond_signal(&_cond);
    }

    ~PthreadPool() 
    {
        for (auto& p : _pthread) 
        {
            delete p;
        }
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
    }

private:
    int _num;
    vector<pthread*> _pthread; // 线程池
    queue<T> _task_queue;      // 任务队列
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
};