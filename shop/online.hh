#pragma once

#include<iostream>
#include<string>
#include<sys/types.h>
#include <sys/socket.h>
#include<cstring>
#include<cstdlib>
#include<pthread.h>
#include <cerrno> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include<functional>
using namespace std;

class UdpOnline
{
    using fun_t=function<void(string,uint16_t,string,int)>;
public:
    UdpOnline(fun_t fun,uint16_t port=8080)
        : _port(port),_sockfd(-1),_fun(fun)
    {}
    void initServer()
    {
        _sockfd=socket(AF_INET,SOCK_DGRAM,0);
        if(_sockfd==-1)
        {
            cerr<<"socket error"<<errno<<":"<<strerror(errno)<<endl;
        }
        struct sockaddr_in local;
        bzero(&local,sizeof local);
        local.sin_family=AF_INET;
        local.sin_port=htons(_port);
        local.sin_addr.s_addr=htonl(INADDR_ANY);
        int n=bind(_sockfd,(struct sockaddr*)&local,sizeof (local));
        if(n==-1)
        {
            cerr<<"Bind Error"<<errno<<":"<<strerror(errno)<<endl;
        }
    }
    void start()
    {
        char buff[1024];
        while (true)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            ssize_t n= recvfrom(_sockfd, buff,sizeof(buff)-1,0,(struct sockaddr*)&peer,&len);
            if(n>0)
            {
                buff[n]=0;
                string clientip=inet_ntoa(peer.sin_addr);
                uint16_t clientport=ntohs(peer.sin_port);
                _fun(clientip,clientport,buff,_sockfd);
            }
        }
    }
private:
    uint16_t _port;
    int _sockfd;
    fun_t _fun;
};