#include"TCPClient.hh"
#include<memory>

int main(int argc,char*argv[])
{
    if(argc!=3)
    {
        cout<<"\nUser\n\t"<<argv[0]<<"Serverip Serverport"<<endl;
        exit(1);
    }
    uint16_t port=stoi(argv[2]);
    unique_ptr<TCPClient> tc(new TCPClient(argv[1],port));
    tc->init();
    tc->run();
    return 0;
}