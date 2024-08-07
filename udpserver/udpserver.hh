#pragma once

#include<iostream>
#include<string>
#include<sys/types.h>
#include <sys/socket.h>
#include<cstring>
#include<cstdlib>
#include <cerrno> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include<functional>
using namespace std;
namespace hzz
{
    static string defaultip="0.0.0.0";
    static int num=1024;
    enum {USER_ERR=1,SOCKET_ERR,BIND_ERR};
    class udpserver
    {
       using fun_t=function<void(int,string&,uint16_t&,string& message)>;
    public:
        udpserver(fun_t fun,const uint16_t port,const string ip=defaultip)
            : _callback(fun),_ip(ip),_port(port),_sockfd(-1)
            {}
        void init()
        {
            //创建服务器
            _sockfd=socket(AF_INET,SOCK_DGRAM,0);
            if(_sockfd<0)
            {
                cerr<<"socket error"<<errno<<strerror(errno)<<endl;
                exit(SOCKET_ERR);
            }
            //开始绑定
            struct sockaddr_in local;
            bzero(&local,sizeof(local));
            local.sin_family=AF_INET;
            local.sin_port=htons(_port);
            local.sin_addr.s_addr=htonl(INADDR_ANY);
            int n=bind(_sockfd,(struct sockaddr*)&local,sizeof(local));
            if(n<0)
            {
                cerr<<"bind errno"<<errno<<strerror(errno)<<endl;
                exit(BIND_ERR);
            }
        }
        void run()
        {
            char buffer[num];
            while(true)//服务器本质就是死循环
            {
                struct sockaddr_in peer;
                socklen_t len=sizeof(peer);
                int n=recvfrom(_sockfd,buffer,sizeof(buffer),0,(struct sockaddr*)&peer,&len);
                if(n>0)
                {
                    buffer[n-1]=0;
                    string clientip=inet_ntoa(peer.sin_addr);
                    uint16_t clientport=ntohs(peer.sin_port);
                    string message=buffer;
                    cout<<"clinet:"<<clientip<<":"<<clientport<<":"<<message<<endl;
                    _callback(_sockfd,clientip,clientport,message);
                }
            }
        }
    private:
        string _ip;
        uint16_t _port;
        int _sockfd;
        fun_t _callback;
    };
}