#include<iostream>
#include<pthread.h>
#include<queue>
using namespace std;
template<class T>
class BlockQueue
{
public:
    BlockQueue(const int maxcap=5)
        :_maxcap(maxcap)
    {
        pthread_mutex_init(&_mutex,nullptr);
        pthread_cond_init(&_ccond,nullptr);
        pthread_cond_init(&_pcond,nullptr);

    }
    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_ccond);
        pthread_cond_destroy(&_pcond);
    }
    void push(const T& in)
    {
        pthread_mutex_lock(&_mutex);
        //细节1：不用if，是防止伪唤醒，如果使用boardcast唤醒会将全部线程唤醒，但是空位置比他们少，就存在伪唤醒
        while(Is_full())
        {
            //细节2：&_mutex是我们正在使用的互斥
            //a.当进行改函数调用时，会以原子性操作将锁释放，并将自己挂起
            //b.被唤醒时，会自动获取传入的锁
            pthread_cond_wait(&_pcond,&_mutex);//因为不满足生产条件，所以将生产者进行等待
        }
        //走到这一步，必然有空位置，可以开始生产
        _q.push(in);
        //细节3:signal可以放在解锁后
        pthread_cond_signal(&_ccond);//唤醒消费者,这里可以有策略唤醒，如生产了多少才唤醒
        pthread_mutex_unlock(&_mutex);
    }
    void pop(T* out)
    {
        pthread_mutex_lock(&_mutex);
        while(Is_empty())
        {
            pthread_cond_wait(&_ccond,&_mutex);
        }
        //走到这里必然有资源
        *out=_q.front();
        _q.pop();
        pthread_cond_signal(&_pcond);//唤醒生产者，这里也可以有策略
        pthread_mutex_unlock(&_mutex);
    }
private:
    bool Is_empty()
    {
        return _q.empty();
    }
    bool Is_full()
    {
        return _q.size()==_maxcap;
    }
private:
    pthread_mutex_t _mutex;
    pthread_cond_t _ccond;
    pthread_cond_t _pcond;
    queue<T> _q;
    int _maxcap;//队列元素最大个数
};