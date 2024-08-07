#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <vector>
#include <string>
#include"threadpool.hh"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <functional>
#include <cstring>
#include <cstdlib>
#include <memory>
#define gbacklog 5
using namespace std;

class TCPServer;
class Task{
     using fun_c = function<void(int)>;
public:
    Task()
    {}
    Task(int sockfd, fun_c func)
        : _sockfd(sockfd), _func(func)
    {}

    void operator()()
    {
        _func(_sockfd);
        close(_sockfd);
    }

private:
    int _sockfd;
    fun_c _func;
};

class ThreadData
{
public:
    ThreadData(TCPServer *th, int sockfd)
        : _tp(th), _sockfd(sockfd)
    {
    }
    TCPServer *_tp;
    int _sockfd;
};
class TCPServer
{
    using fun_c = function<void(int)>;

public:
    TCPServer(uint16_t port, fun_c fun)
        : _port(port), _fun(fun), _listensockfd(-1),pool(10)
    {}
    static void *threadroutine(void *arg)
    {
        ThreadData *td(static_cast<ThreadData *>(arg));
        td->_tp->_fun(td->_sockfd);
        close(td->_sockfd);
        delete td;
        td = nullptr;
        return nullptr;
    }
    void init()
    {
        // 1.获取监听套接字
        _listensockfd = socket(AF_INET, SOCK_STREAM, 0);
        // 2.处理ip，port
        struct sockaddr_in local;
        local.sin_addr.s_addr = htonl(INADDR_ANY);
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        // 3.bind
        if (bind(_listensockfd, (struct sockaddr *)&local, sizeof(local)) == 1)
        {
            cerr << "bind ERROR" << errno << ":" << strerror(errno) << endl;
            exit(1);
        }
        // 4.监听listen
       if (listen(_listensockfd, gbacklog) < 0)
        {
            cerr << "listen ERROR " << errno << ": " << strerror(errno) << endl;
            exit(2);
        }
    }
    void run()
    {

        while (true)
        {
            // 1.获取套接字，accept
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int sockfd = accept(_listensockfd, (struct sockaddr *)&peer, &len);
            if (sockfd < 0)
            {
                cerr << "accept ERROR" << errno << strerror(errno) << endl;
                continue;
            }
            // 2.处理信息
            // 2.1.多进程处理
            //  pid_t id=fork();
            //  if(id==0)//子进程
            //  {
            //      close(_listensockfd);//防止子进程误操作
            //      if(fork()>0) exit(0);//关闭子进程
            //      //孙子进程处理，防止父进程阻塞
            //      _fun(sockfd);
            //      close(sockfd);//不是完全关闭sockfd，只是减少计数值
            //  }
            // 2.2.多线程处理
            // pthread_t id;
            // ThreadData *td = new ThreadData(this, sockfd);
            // pthread_create(&id, nullptr, threadroutine, td);
            //3.线程池处理
             pool.push(Task(sockfd, _fun));
            // 3.关闭sockfd
            //  pid_t ret=waitpid(id,nullptr,0);
            //  if(ret>0)
            //  {
            //      cout<<"waitsucess"<<ret<<endl;
            //  }
            //close(sockfd); // 父进程和孙子进程都执行才是真正关闭
        }
    }

private:
    int _listensockfd;
    uint16_t _port;
    fun_c _fun;
    threadpool<Task> pool;
};