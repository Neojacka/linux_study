#include"online.hh"
#include<memory>

static int count=100;
void handlermessage(string ip,uint16_t port,string cmd,int sockfd)
{
    struct sockaddr_in back;
    bzero(&back,sizeof(back));
    back.sin_addr.s_addr=inet_addr(ip.c_str());
    back.sin_family=AF_INET;
    back.sin_port=htons(port);
    string responce="欢迎来到理工超市\n请选择你要进行的操作\n\t1.登入  2.注册 3.购物车 4.退出";
    sendto(sockfd,responce.c_str(),responce.size(),0,(struct sockaddr*)&back,sizeof(back));
    
}

int main(int argc,char*argv[])
{
    if(argc<2)
    {
        cout<<"\nUsage:\n\t"<<argv[0]<<"loacl_port\n\n";
        exit(-1);
    }
    uint16_t port=atoi(argv[1]);
    unique_ptr<UdpOnline> usvr(new UdpOnline(handlermessage,port));
    usvr->initServer();
    usvr->start();

    return 0;
}