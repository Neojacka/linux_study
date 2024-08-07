#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <vector>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <functional>
#include <cstring>
#include <cstdlib>
#include "Unit.hh"
#include "logMessage.hh"
#define gbacklog 5
using namespace std;

class TCPServer
{
    using fun_c = function<void(Request, Response &)>;

public:
    TCPServer(uint16_t port, fun_c fun)
        : _port(port), _fun(fun), _listensockfd(-1)
    {
    }
    void handler(int sockfd)
    {
        string inbuff;
        while (true)
        {
            // 1.确保接收到一个完整报文
            string text,str;
            if(!recvrequest(sockfd,&text,inbuff)) return;
            cout<<"报文："<<text;
            // 2.解析报文 
            if(!delLength(text,&str)) return;
            cout<<"序列化的请求"<<str;
            // 3.反序列化解析后的报文
            Request req;
            if(!req.Unserialize(str)) return;
            // 4.对请求做出回应
            Response resp;
            _fun(req, resp);
            // 5.将回应进行序列化操作
            string respon;
            resp.Serialize(&respon);
            // 6.将序列化后的回应形成报文发送
            respon=enLength(respon);
            write(sockfd, respon.c_str(), respon.size());
        }
    }
    void init()
    {
        // 1.获取监听套接字
        _listensockfd = socket(AF_INET, SOCK_STREAM, 0);
        logMessage(NORMAL, "获取监听套接字:%d成功", _listensockfd);
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
        logMessage(NORMAL, "bind:%d成功", _listensockfd);
        // 4.监听listen
        if (listen(_listensockfd, gbacklog) < 0)
        {
            cerr << "listen ERROR " << errno << ": " << strerror(errno) << endl;
            exit(2);
        }
        logMessage(NORMAL, "监听套接字:%d正在监听", _listensockfd);
    }
    void run()
    {

        while (true)
        {
            // 1.获取套接字，accept
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int sockfd = accept(_listensockfd, (struct sockaddr *)&peer, &len);
            cout<<"连接成功！连接者："<<sockfd<<endl;
            if (sockfd < 0)
            {
                logMessage(ERROR, "获取套接字失败");
                continue;
            }
            logMessage(NORMAL, "获取套接字:%d成功", sockfd);
            // 2.处理信息
            pid_t id = fork();
            pid_t pd;
            if (id == 0) // 子进程
            {
                pd = getpid();
                close(_listensockfd); // 防止子进程误操作
                if (fork() > 0)
                    exit(0); // 关闭子进程
                // 孙子进程处理，防止父进程阻塞
                handler(sockfd);
                close(sockfd); // 不是完全关闭sockfd，只是减少计数值
            }
            // 3.关闭sockfd
            pid_t ret = waitpid(id, nullptr, 0);
            if (ret > 0)
            {
                logMessage(NORMAL, "等待子进程:%d成功", pd);
            }
            close(sockfd); // 父进程和孙子进程都执行才是真正关闭
        }
    }

private:
    int _listensockfd;
    uint16_t _port;
    fun_c _fun;
};