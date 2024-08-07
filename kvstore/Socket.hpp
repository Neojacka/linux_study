#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include"logMessage.hh"
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
        Socket(){}
        void Socketfd()
        {
            _sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
            {
                cerr << "SOCKET ERRNO " << errno << strerror(errno) << endl;
                exit(SOCKETERR);
            }
            // 设置端口复用，避免timewait阻塞端口
            int opt = 1;
            setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        }
        void Bind(uint16_t port)
        {
            struct sockaddr_in local;
            bzero(&local, sizeof(local));
            local.sin_addr.s_addr = INADDR_ANY;
            local.sin_family = AF_INET;
            local.sin_port = htons(port);
            if (bind(_sockfd, (struct sockaddr *)&local, sizeof(local)) == -1)
            {
                cerr << "BIND ERRNO " << errno << strerror(errno) << endl;
                exit(BINDERR);
            }
        }

        void Listen()
        {
            if (listen(_sockfd, gbacklog) < 0)
            {
                cerr << "LISTEN ERRNO " << errno << strerror(errno) << endl;
                exit(LISTENERR);
            }
        }
        int Accept(string *clientip, uint16_t *clientport, int *err)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int sockfd = accept(_sockfd, (struct sockaddr *)&peer, &len);
            if (sockfd < 0)
            {
                *err = errno;
                return -1;
            }
            if (clientip)
                *clientip = inet_ntoa(peer.sin_addr);
            if (clientport)
                *clientport = ntohs(peer.sin_port);
            return sockfd;
        }
        void Connect(string serveip, uint16_t port)
        {
            // 连接
            struct sockaddr_in clocal;
            bzero(&clocal, sizeof(clocal));
            clocal.sin_addr.s_addr = inet_addr(serveip.c_str());
            clocal.sin_family = AF_INET;
            clocal.sin_port = htons(port);

            int n = connect(_sockfd, (struct sockaddr *)&clocal, sizeof(clocal));
            if (n < 0)
            {
                cerr << "Connect ERRNO " << errno << strerror(errno) << endl;
                exit(2);
            }
        }
        int Sockfd()
        {
            return _sockfd;
        }
        ~Socket()
        {
            if (_sockfd != -1)
                close(_sockfd);
        }

    private:
        int _sockfd;
    };
}
