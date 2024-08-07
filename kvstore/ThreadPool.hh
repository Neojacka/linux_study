#pragma once
#include <iostream>
#include <thread>
#include <condition_variable>
#include <memory>
#include <functional>
#include <vector>
#include <string>
#include <queue>
#include "kvstore.hh"
using namespace std;

class ThreadPool
{
    using fun_t = function<void()>;

private:
    void fun()
    {
        thread::id thread_id = this_thread::get_id();
        while (true)
        {
            fun_t task;
            {
                unique_lock<mutex> ul(_mutex);
                _cv.wait(ul, [this]()
                         { return !_task.empty(); });
                // 从任务队列中取出任务
                task = _task.front();
                _task.pop();
            }
            // 执行任务
            if (task)
            {
                task();
                cout<<"执行线程ID："<<thread_id<<endl;
            }
        }
    }

public:
    ThreadPool(int num)
        : _num(num)
    {
        for (int i = 0; i < _num; i++)
        {
            work.emplace_back(thread(&ThreadPool::fun, this));
        }
    }
    ~ThreadPool()
    {
        for (thread &t : work)
        {
            if (t.joinable())
                t.join();
        }
    }
    void addtask(fun_t fun) // 注意这里改为值传递
    {
        {
            lock_guard<mutex> lk(_mutex);
            _task.push(std::move(fun)); // 使用 std::move
        }
        _cv.notify_one();
    }

private:
    int _num;
    vector<thread> work;
    queue<fun_t> _task;
    condition_variable _cv;
    mutex _mutex;
};
