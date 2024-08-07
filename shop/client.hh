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
using namespace std;
enum {USER_ERR=1,SOCKET_ERR,BIND_ERR};
class udpclient
{
public:
    udpclient(const string serverip,const uint16_t serverport)
        : _serverip(serverip),_serverport(serverport),_sockfd(-1)
    {}
    void init()
    {
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd < 0)
        {
            cerr << "socket error" << errno << strerror(errno) << endl;
            exit(SOCKET_ERR);
        }
        //客户端不需要绑定ip与端口
    }
    static void* readmessage(void*args)
    {
        int sockfd=*(static_cast<int*>(args));
        pthread_detach(pthread_self());
        while(true)
        {
            char buffer[1024];
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&peer, &len);
            if (n > 0)
            {
                buffer[n] = 0;
                cout << buffer << endl;
            }
        }
    }
    void run()
    {
        pthread_create(&read,nullptr,readmessage,(void*)&_sockfd);
        struct sockaddr_in server;
        bzero(&server,sizeof(server));
        server.sin_family=AF_INET;
        server.sin_port=htons(_serverport);
        server.sin_addr.s_addr=inet_addr(_serverip.c_str());
        string message;
        char cmdline[1024];
        while(true)
        {
            fprintf(stderr,"Enter:");
            fflush(stderr);
            fgets(cmdline,sizeof(cmdline),stdin);
            message=cmdline;
            sendto(_sockfd,message.c_str(),message.size(),0,(struct sockaddr*)&server,sizeof(server));
        }
    }
private:
    string _serverip;
    uint16_t _serverport;
    int _sockfd;
    pthread_t read;
};