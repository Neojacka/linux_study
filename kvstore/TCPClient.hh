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
#include "Util.hpp"
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
            exit(1);
        }
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
        string str, inbuff;
        Response resp;
        while (true)
        {
            cout<<"请输入：";
            getline(cin, mes);
            send(_sockfd, mes.c_str(), mes.size(),0);

            char buff[1024];
            int n=recv(_sockfd,buff,sizeof(buff),0);
            if(n>0)
            {
                buff[n]=0;
                cout<<buff<<endl;
            }
        }
        close(_sockfd);
    }

private:
    int _sockfd;
    string _ip;
    uint16_t _port;
};