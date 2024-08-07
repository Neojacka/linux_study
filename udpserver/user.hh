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
#include<unordered_map>
using namespace std;

class user {
public:
    user(const string& ip, uint16_t port) : _ip(ip), _port(port) {}
    user(const user& u) : _ip(u._ip), _port(u._port) {}

    string ip() const { return _ip; }
    uint16_t port() const { return _port; }

private:
    string _ip;
    uint16_t _port;
};

class onlineuser 
{
public:
    onlineuser() {}
    ~onlineuser() {}

    void adduser(const string& ip, uint16_t& port) 
    {
        string id = ip + "-" + to_string(port);
        users.insert({id, user(ip, port)});
    }

    void deluser(const string& ip, uint16_t& port) 
    {
        string id = ip + "-" + to_string(port);
        users.erase(id);
    }

    bool isonline(const string& ip, uint16_t &port) 
    {
        string id = ip + "-" + to_string(port);
        return users.find(id) != users.end();
    }
    void broadmessage(int sockfd, const string& ip, uint16_t port,string& message) 
    {
        for(auto& user:users)
        {
            struct sockaddr_in client;
            bzero(&client,sizeof(client));
            client.sin_family=AF_INET;
            client.sin_port=htons(user.second.port());
            client.sin_addr.s_addr=inet_addr(user.second.ip().c_str());
            string s =ip+"-"+to_string(port)+":";
            s+=message;
            sendto(sockfd,s.c_str(),s.size(),0,(struct sockaddr*)&client,sizeof(client));
        }
    }

private:
    unordered_map<string, user> users;
};
