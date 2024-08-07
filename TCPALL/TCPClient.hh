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
#include <sstream>
#define NUM 1024
using namespace std;

class TCPClient
{
public:
    TCPClient(string ip, uint16_t port)
        : _ip(ip), _port(port), _sockfd(-1)
    {
    }
    void init()
    {
        // 获取套接字
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_sockfd < 0)
        {
            logMessage(NORMAL, "进程:%d获取套接字失败", getpid());
            exit(1);
        }
        logMessage(NORMAL, "进程:%d获取套接字:%d成功", getpid(), _sockfd);
        // 连接
        struct sockaddr_in clocal;
        bzero(&clocal, sizeof(clocal));
        clocal.sin_addr.s_addr = inet_addr(_ip.c_str());
        clocal.sin_family = AF_INET;
        clocal.sin_port = htons(_port);

        int n = connect(_sockfd, (struct sockaddr *)&clocal, sizeof(clocal));
        if (n < 0)
        {
            logMessage(ERROR, "进程：%d建立连接失败", getpid());
            exit(2);
        }
    }
    void run()
    {
        string mes;
        string str,inbuff;
        Response resp;
        while (true)
        {
            getline(cin, mes);
            stringstream ss(mes);
            Request req;
            ss >> req._x >> req._y >> req._op;
            req.Serialize(&mes);
            mes=enLength(mes);
            write(_sockfd, mes.c_str(), mes.size());

             // 1.确保接收到一个完整报文
            string text,str;
            if(!recvrequest(_sockfd,&text,inbuff)) return;
            // 2.解析报文 
            if(!delLength(text,&str)) return;
            // 3.反序列化解析后的报文
            Response resp;
            if(!resp.Unserialize(str)) return;;
            cout << "反序列化后的数据:" << "进程退出码："<<resp._exitcode<<" "<<"计算结果："<<resp._calnum<< endl;
        }
        close(_sockfd);
    }

private:
    int _sockfd;
    string _ip;
    uint16_t _port;
};