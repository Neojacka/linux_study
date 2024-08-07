#pragma once
#include <iostream>
#include "Socket.hpp"
#include "epoller.hpp"
#include <string>
#include <vector>
#include <fcntl.h>
#include <sstream>
#include <unordered_map>
#include <map>
#include <functional>
#include "logMessage.hh"
#include "ThreadPool.hh"
#include <mutex>
#include <memory>
#define defaultnum 1024
using namespace std;
using namespace hzz;

class TCPServer;
// 每个文件描述符都要有自己的读写缓冲区和对应处理方法
class Connection
{
    using fun_t = function<void(Connection *)>;

public:
    Connection(int sockfd, TCPServer *t)
        : _sockfd(sockfd), ts(t)
    {
    }
    void Register(fun_t recver, fun_t sender, fun_t except)
    {
        _recver = recver;
        _sender = sender;
        _except = except;
    }

public:
    string _inbuff;
    string _outbuff;
    int _sockfd;
    TCPServer *ts;
    fun_t _recver;
    fun_t _sender;
    fun_t _except;
    mutex _rmutex;
    mutex _wmutex;
};

class TCPServer
{
    using handlt_t = function<void(Connection *, string &)>;
    using fun_t = function<void(Connection *)>;

private:
    void EableReadWrite(Connection *conn, bool read, bool write)
    {
        uint32_t event = (read ? EPOLLIN : 0) | (write ? EPOLLOUT : 0) | EPOLLET;
        _ep.Epoll_Ctr(conn->_sockfd, event, EPOLL_CTL_MOD);
    }

    void recever(Connection *conn)
    {
#ifdef DEBUG
        cout << "recever come" << endl;
#endif

        while (true)
        {

            char buff[1024];
            ssize_t n = 0;
            {
                lock_guard<mutex> lock(conn->_rmutex);
                n = recv(conn->_sockfd, buff, sizeof(buff), 0);
            }

            if (n > 0)
            {

                buff[n] = 0;
                string data=buff;
                _service(conn, data);
            }
            // 连接关闭
            else if (n == 0)
            {
                logMessage(NORMAL, "sockfd[%d]断开链接", conn->_sockfd);
                conn->_except(conn);
                return;
            }
            else
            {
                // 接收缓冲区空了
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break;
                // 信号中断了
                else if (errno == EINTR)
                    continue;
                // 真正出问题了
                else
                {
                    conn->_except(conn);
                    logMessage(NORMAL, "读取[%d]信息失败", conn->_sockfd);
                    return;
                }
            }
        }
    }
    void sender(Connection *conn)
    {
#ifdef DEBUG
        cout << "sende come" << endl;
        cout << conn->_outbuff << endl;
#endif
        lock_guard<mutex> lock(conn->_wmutex);
        while (true)
        {
            ssize_t s = send(conn->_sockfd, conn->_outbuff.c_str(), conn->_outbuff.size(), 0);
            if (s > 0)
            {
                conn->_outbuff.erase(0, s);
                logMessage(NORMAL, "发送信息给[%d]成功", conn->_sockfd);
                // 发送缓冲区空了，只让epoll关注读事件
                if (conn->_outbuff.empty())
                {
                    EableReadWrite(conn, true, false);
                    break;
                }
                else
                {
                    EableReadWrite(conn, true, true);
                }
            }
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break;
                // 信号中断了
                else if (errno == EINTR)
                    continue;
                // 真正出问题了
                else
                {
                    logMessage(NORMAL, "发送信息给[%d]失败", conn->_sockfd);
                    conn->_except(conn);
                    return;
                }
            }
        }
    }
    void except(Connection *conn)
    {
        // 1.先判断有效性
        if (ISExit(conn->_sockfd))
            return;
        // 2.服务器上的所有异常都在这里了
        // 2.1先删除epoll的监控
        _ep.Epoll_Ctr(conn->_sockfd, EPOLLIN | EPOLLOUT, EPOLL_CTL_DEL);
        // 2.2关闭sockfd
        close(conn->_sockfd);
        _connections.erase(conn->_sockfd);
        delete conn;
    }
    void addConnection(int sockfd, uint32_t event, fun_t recver, fun_t sender, fun_t except)
    {
        // 1.先判断是ET模式吗
        if (event & EPOLLET)
        {
            int fl = fcntl(sockfd, F_GETFL);
            fcntl(sockfd, F_SETFL, fl | O_NONBLOCK);
        }
        // 2.创建COnnection对象
        Connection *conn = new Connection(sockfd, this);
        conn->Register(recver, sender, except);
        // 3.添加到epoll模型中,让它关心事件
        _ep.AddEvent(sockfd, event);
        _connections.insert(make_pair(sockfd, conn));
        logMessage(NORMAL, "addConnection sockfd[%d] sucessful", sockfd);
    }
    void Accepter(Connection *conn)
    {
        // ET模式只通知一次-->必须全部取走数据-->循环
        while (true)
        {
            string clientip;
            uint16_t clientport;
            int err = 0;
            // 循环读取-->必然有一次不成功-->Accept不能有exit()
            int sockfd = _sock.Accept(&clientip, &clientport, &err);
            if (sockfd > 0)
            {
                addConnection(sockfd, EPOLLIN | EPOLLET,
                              bind(&TCPServer::recever, this, placeholders::_1),
                              bind(&TCPServer::sender, this, placeholders::_1),
                              bind(&TCPServer::except, this, placeholders::_1));
                logMessage(NORMAL, "sockfd[%d]建立链接成功", sockfd);
            }
            else
            {
                if (err == EAGAIN || err == EWOULDBLOCK)
                    break;
                if (err == EINTR)
                    continue;
                else
                {
                    if (conn->_except)
                    {
                        conn->_except(conn);
                        logMessage(ERROR, "建立链接fault");
                        break;
                    }
                }
            }
        }
    }

public:
    TCPServer(uint16_t port, handlt_t service)
        : _port(port), _service(service), _num(defaultnum), _threadPool(5)
    {
    }
    void init()
    {
        // 1.建立网络套接字和绑定
        _sock.Socketfd();
        _sock.Bind(_port);
        _sock.Listen();
        // 2.建立epoll模型
        _ep.Createpoll();
        // 3.将listenfd也封装成connection对象
        addConnection(_sock.Sockfd(), EPOLLIN | EPOLLET,
                      bind(&TCPServer::Accepter, this, std::placeholders::_1),
                      nullptr, nullptr);
        // 4.建立监听队列
        _revs = new struct epoll_event[_num];
        logMessage(NORMAL, "服务器初始化完成，listenfd:%d", _sock.Sockfd());
    }
    bool ISExit(int sock)
    {
        auto pos = _connections.find(sock);
        if (pos == _connections.end())
            return false;
        else
            return true;
    }
    void Loop(int timeout)
    {
        int n = _ep.Epoll_Wait(_revs, _num, timeout);
        for (int i = 0; i < n; i++)
        {
            int sock = _revs[i].data.fd;
            uint32_t event = _revs[i].events;
#ifdef DEBUG
            cout << "come" << endl;
#endif
            // 将所有的异常问题，全部转换位读写问题
            if (event & EPOLLERR)
                event |= (EPOLLIN | EPOLLOUT);
            if (event & EPOLLHUP)
                event |= (EPOLLIN | EPOLLOUT);
            // 使用线程池处理事件
            if (event & EPOLLIN && ISExit(sock) && _connections[sock]->_recver)
            {
                _threadPool.addtask([this, sock]()
                                    { _connections[sock]->_recver(_connections[sock]); });
            }
            if (event & EPOLLOUT && ISExit(sock) && _connections[sock]->_sender)
            {
                _threadPool.addtask([this, sock]()
                                    { _connections[sock]->_sender(_connections[sock]); });
            }
        }
    }
    void Dispatcher()
    {
        while (true)
        {
            Loop(1000);
        }
    }
    ~TCPServer()
    {
        delete[] _revs;
        for (auto &c : _connections)
        {
            delete c.second;
        }
    }

private:
    epoller _ep;
    Socket _sock;
    uint16_t _port;
    struct epoll_event *_revs;
    unordered_map<int, Connection *> _connections;
    int _num; // 完成队列长度
    handlt_t _service;
    ThreadPool _threadPool;
};