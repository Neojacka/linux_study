#pragma once
#include <thread>
#include <memory>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>

using namespace std;

#define num 5 // 线程池线程数量


template<class T>
class threadpool {
    using fun_t = function<void(void*)>;

private:
    void fun()
    {
        thread::id id = this_thread::get_id();//获取线程id
        while (true)
        {
            T task;
            {
                unique_lock<mutex> lk(mut);//lock_guard不支持随时释放，无法与wait结合使用
                //lambda 表达式创建了一个新的作用域，需要访问 this 指针来引用类的成员变量。
                cv.wait(lk, [this] { return !this->task_queue.empty(); });
                task = task_queue.front();
                task_queue.pop();
            }
            // 执行任务不需要加锁，提高运行效率
            task();
            cout << "线程：" << id << " 完成任务 "<<endl;
            
        }
    }

public:
    threadpool(int count = num)
        : _count(count)
    {
        for (int i = 0; i < _count; i++)
        {
            //成员函数作为线程的入口函数，你需要提供类的实例指针以及成员函数指针,普通函数不用
            work.push_back(thread(&threadpool::fun, this)); 
        }
    }

    ~threadpool()
    {
        for (thread& t : work) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    void push(T t)
    {
        {
            lock_guard<mutex> lock(mut);
            task_queue.push(t);
        }
        cv.notify_one();
    }

private:
    vector<thread> work; // 线程池
    int _count; // 线程数量
    queue<T> task_queue; // 任务队列
    mutex mut;
    condition_variable cv;
};
