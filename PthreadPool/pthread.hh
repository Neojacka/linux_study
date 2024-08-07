#pragma once
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <functional>
using namespace std;
namespace hzz
{
    class pthread
    {
        using fun_t = function<void *(void *)>;

    private:
        static void *start_routine(void *argc)
        {
            pthread *_this = static_cast<pthread *>(argc);
            return _this->callback();
        }

    public:
        pthread()
        {
            char namebuff[1024];
            snprintf(namebuff, sizeof(namebuff), "pthread-%d正在执行", pthreadnum++);
            _name = namebuff;
        }
        // 运行线程
        void start(fun_t fun, void *argc = nullptr)
        {
            _callback=fun;
            _argc = argc;
            int n = pthread_create(&tid, nullptr, start_routine, this);
        }
        void *callback()
        {
            return _callback(_argc);
        }
        // 阻塞等待
        void join()
        {
            pthread_join(tid, nullptr);
        }
        string& getname()
        {
            return _name;
        }
        ~pthread()
        {
        }

    private:
        fun_t _callback;
        pthread_t tid;
        void *_argc;
        string _name;
        static int pthreadnum;
    };
    int pthread::pthreadnum = 1;
}
