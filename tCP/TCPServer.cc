#include"TCPServer.hh"
#include<memory>

void fun(int sockfd)
{
    char buff[1024];
    while (true)
    {
        ssize_t n = read(sockfd, buff, sizeof(buff) - 1);
        if (n > 0)
        {
            buff[n] = 0; // 清除\n
            cout << buff << endl;
            string respon = "Server#";
            respon += buff;
            write(sockfd, respon.c_str(), respon.size());
        }
        else{
            cout<<"clien退出了，我也退出"<<endl;
            exit(0);
        }
    }
}

int main(int argc,char*argv[])
{
    if(argc!=2)
    {
        cout<<"\nUsage:\n\t"<<argv[0]<<"loacl_port"<<endl;
        exit(0);
    }
    uint16_t port =stoi(argv[1]);
    unique_ptr<TCPServer> ts(new TCPServer(port,fun));
    ts->init();
    ts->run();
}