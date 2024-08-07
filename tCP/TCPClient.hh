#include<iostream>
#include<unistd.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <sys/wait.h>
#include<vector>
#include<string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<functional>
#include<cstring>
#include<cstdlib>
#define NUM 1024
using namespace std;

class TCPClient
{
public:
    TCPClient(string ip,uint16_t port)
        :_ip(ip),_port(port),_sockfd(-1)
    {}
    void init()
    {
        //获取套接字
        _sockfd=socket(AF_INET,SOCK_STREAM,0);
        if(_sockfd<0)
        {
             cerr<<"socket ERROR"<<errno<<":"<<strerror(errno)<<endl;
             exit(1);
        }
        //连接
          struct sockaddr_in clocal;
        bzero(&clocal, sizeof(clocal));
        clocal.sin_addr.s_addr = inet_addr(_ip.c_str());
        clocal.sin_family = AF_INET;
        clocal.sin_port = htons(_port);

        int n = connect(_sockfd, (struct sockaddr *)&clocal, sizeof(clocal));
        if (n < 0)
        {
            cerr << "connect ERROR " << errno << ": " << strerror(errno) << endl;
            exit(2);
        }
    }
    void run()
    {
        string mes;
        while (true)
        {
            getline(cin, mes);
            write(_sockfd, mes.c_str(), mes.size());

            char buff[NUM];
            int n = read(_sockfd, buff, sizeof(buff) - 1);
            if (n > 0)
            {
                buff[n] = 0;
                cout << buff << endl;
            }
            else
            {
                cerr << "read ERROR " << errno << ": " << strerror(errno) << endl;
                break;
            }
        }
        close(_sockfd);
    }
private:
    int _sockfd;
    string _ip;
    uint16_t _port;
};