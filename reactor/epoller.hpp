#pragma once
#include <iostream>
#include <sys/epoll.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include "logMessage.hh"
using namespace std;
namespace hzz
{
    int size = 128;
    class epoller
    {
    public:
        void Createpoll()
        {
            _eplfd = epoll_create(size);
            if (_eplfd < 0)
            {
                cerr << "EPOLL_CREAT ERRNO" << errno << strerror(errno);
                exit(1);
            }
        }
        bool AddEvent(int sock, uint32_t event)
        {
            struct epoll_event ev;
            ev.data.fd = sock;
            ev.events = event;
            // 添加到epoll中
            int n = epoll_ctl(_eplfd, EPOLL_CTL_ADD, sock, &ev);
            return n;
        }
        bool Epoll_Ctr(int sock, uint32_t event, int action)
        {
            int n = 0;
            if (action == EPOLL_CTL_MOD)
            {
                struct epoll_event ev;
                ev.data.fd = sock;
                ev.events = event;
                n=epoll_ctl(_eplfd,action,sock,&ev);
            }
            else if(action==EPOLL_CTL_DEL)
            {
                 n=epoll_ctl(_eplfd,action,sock,nullptr);
            }
            else
                n==-1;
            return n==0;
        }
        int Epoll_Wait(struct epoll_event *revs, int num, int timeout)
        {
            int n = epoll_wait(_eplfd, revs, num, timeout);
            if (n < 0)
            {
                cerr << "EPOLL_WAIT ERRNO" << errno << strerror(errno) << endl;
            }
            return n;
        }
    ~epoller()
    {
        if(_eplfd!=-1)
            close(_eplfd);
    }
    private:
        int _eplfd;
    };
}