#include<iostream>
#include<vector>
#include<pthread.h>
#include<semaphore.h>
using namespace std;
int cap=10;

template<class T>
class Ringqueue
{
public:
    Ringqueue()
    : _qu(cap),_cap(cap)
    {
        sem_init(&_csem,0,0);//刚开始没有资源
        sem_init(&_psem,0,cap);//刚开始的空位置全在
        pthread_mutex_init(&_pmutex,nullptr);
        pthread_mutex_init(&_cmutex,nullptr);
        p_step=c_step=0;
    }
    ~Ringqueue()
    {
        sem_destroy(&_csem);
        sem_destroy(&_psem);
        pthread_mutex_destroy(&_pmutex);
        pthread_mutex_destroy(&_cmutex);
    }
    void P(sem_t& sem)
    {
        sem_wait(&sem);
    }
    void V(sem_t& sem)
    {
        sem_post(&sem);
    }
    void push(const T& in)
    {
        P(_psem);
        //到这步说明必然有空位置
        pthread_mutex_lock(&_pmutex);
        _qu[p_step++]=in;
        p_step%=_cap;
        pthread_mutex_unlock(&_pmutex);
        V(_csem);//位置没有还回来，多了一份资源
    }
    void pop(T* out)
    {
        P(_csem);
         //到这步说明必然有资源
        pthread_mutex_lock(&_cmutex);
        *out=_qu[c_step++];
        c_step%=_cap;
        pthread_mutex_unlock(&_cmutex);
        V(_psem);//资源没有还回来，多了一个空位置
    }
private:
    sem_t _csem;
    sem_t _psem;
    vector<T> _qu;
    pthread_mutex_t _pmutex,_cmutex;
    int _cap;
    int p_step;
    int c_step;
};