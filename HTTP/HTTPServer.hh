#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <functional>
#include "Uitl.hh"
#include <cstring>
#include <cstdlib>
#include <sys/wait.h>
#define gbacklog 5
using namespace std;
enum
{
    SOCKETERR = 1,
    BINDERR,
    LISTENERR,
    ACCEPTERR,
    READERR,
    WRITEERR
};

class HTTPServer
{
    using fun_c = function<void(Request &, Response &)>;

public:
    HTTPServer(uint16_t port, fun_c fun)
        : _listensock(-1), _port(port), _fun(fun)
    {
    }
    void handler(int sockfd)
    {
        Request req;
        Response resp;
        while (true)
        {
            // 1.读到完整http请求
            // 2.反序列化
            // 3.执行回调函数
            // 4.序列化
            // 5.send
            char buff[4096];
            int n = recv(sockfd, buff, sizeof(buff) - 1, 0);//大概率可以读到完整http请求
            if (n > 0)
            {
                buff[n] = 0;
                req.inbuff = buff;
                 // 4.反序列化
                req.parse();
                _fun(req, resp);
                //funs[req.url](req,resp);//根据请求路径来提供相应的服务
                send(sockfd, resp.outbuff.c_str(), resp.outbuff.size(), 0);
            }
            else if (n == 0)
            {
                cout << "DEBUG: 客户端断开连接" << endl;
                break;
            }
            else
            {
                cerr << "DEBUG: 接收消息失败" << strerror(errno) << endl;
                break;
            }
        }
    }

    void init()
    {
        _listensock = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensock < 0)
        {
            cerr << "socket false" << errno << strerror(errno) << endl;
            exit(SOCKETERR);
        }

        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_addr.s_addr = INADDR_ANY;
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        if (bind(_listensock, (struct sockaddr *)&local, sizeof(local)) < 0)
        {
            cerr << "bind false" << errno << strerror(errno) << endl;
            exit(BINDERR);
        }
        if (listen(_listensock, gbacklog) < 0)
        {
            cerr << "listen false" << errno << strerror(errno) << endl;
            exit(LISTENERR);
        }
    }
    void registerCb(string servicename,fun_c fun)
    {
        funs.insert(make_pair(servicename,fun));
    }
    void run()
    {

        while (true)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int sockfd = accept(_listensock, (struct sockaddr *)&peer, &len);
            if (sockfd < 0)
            {
                cerr << "accept false" << errno << strerror(errno) << endl;
                continue;
            }
            pid_t id = fork();
            if (id == 0)
            {
                close(_listensock);
                if (fork() > 0)
                    exit(0);
                handler(sockfd);
                close(sockfd);
                exit(0);
            }
            int ret = waitpid(id, nullptr, 0);
            if (ret > 0)
            {
                cout << "waitsucessful" << endl;
            }
            close(sockfd);
        }
    }

private:
    int _listensock;
    uint16_t _port;
    fun_c _fun;
    unordered_map<string,fun_c> funs;
};