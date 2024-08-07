#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;
#define gbacklog 5
#define defaultfd -1
namespace hzz
{
    enum
    {
        SOCKETERR,
        BINDERR,
        LISTENERR,
        ACCEPTERR
    };
    class Socket
    {
    public:
        void Socketfd()
        {
            _listensock = socket(AF_INET, SOCK_STREAM, 0);
            if (_listensock < 0)
            {
                cerr << "SOCKET ERRNO " << errno << strerror(errno) << endl;
                exit(SOCKETERR);
            }
            // 设置端口复用，避免timewait阻塞端口
            int opt = 1;
            setsockopt(_listensock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        }
        void Bind(uint16_t port)
        {
            struct sockaddr_in local;
            bzero(&local, sizeof(local));
            local.sin_addr.s_addr = INADDR_ANY;
            local.sin_family = AF_INET;
            local.sin_port = htons(port);
            if (bind(_listensock, (struct sockaddr *)&local, sizeof(local)) == -1)
            {
                cerr << "BIND ERRNO " << errno << strerror(errno) << endl;
                exit(BINDERR);
            }
        }

        void Listen()
        {
            if (listen(_listensock, gbacklog) < 0)
            {
                cerr << "LISTEN ERRNO " << errno << strerror(errno) << endl;
                exit(LISTENERR);
            }
        }
        int Accept(string *clientip, uint16_t *clientport,int* err)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int sockfd = accept(_listensock, (struct sockaddr *)&peer, &len);
            if (sockfd < 0)
            {
                *err=errno;
                return -1;
            }
            if (clientip)
                *clientip = inet_ntoa(peer.sin_addr);
            if (clientport)
                *clientport = ntohs(peer.sin_port);
            return sockfd;
        }
        int Sockfd()
        {
            return _listensock;
        }
    ~Socket()
    {
        if(_listensock!=-1)
            close(_listensock);
    }
    private:
        int _listensock;
    };
}
