#include"udpserver.hh"
#include"user.hh"
#include<memory>
using namespace hzz;

onlineuser users;
void Usage(string por)
{
    cout<<"\nUsage:\n\t"<<por<<"local_port\n\n";
}
void onlinechat(int sockfd,string& ip,uint16_t& port,string& message)
{
    if(message=="online")
        users.adduser(ip,port);
    if(message=="offline")
        users.deluser(ip,port);
    if(users.isonline(ip,port))
    {
        users.broadmessage(sockfd,ip,port,message);
    }
    else
    {
        struct sockaddr_in client;
        bzero(&client, sizeof(client));
        client.sin_family = AF_INET;
        client.sin_port = htons(port);
        client.sin_addr.s_addr = inet_addr(ip.c_str());
        string responmessage = "你还没有上线，请上线(输入)：online";
        sendto(sockfd, responmessage.c_str(), responmessage.size(), 0, (struct sockaddr *)&client, sizeof(client));
    }
}
int main(int argc,char*argv[]) 
{
    if(argc<2)
    {
        Usage(argv[0]);
        exit(USER_ERR);
    }
    uint16_t port=atoi(argv[1]);
    unique_ptr<udpserver> usev(new udpserver(onlinechat,port));
    usev->init();
    usev->run();
    return 0;
}