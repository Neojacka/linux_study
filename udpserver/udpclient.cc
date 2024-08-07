#include"udpclient.hh"
#include<memory>
void Usage(string por)
{
    cout<<"\nUsage:\n\t"<<por<<"server_ip server_port\n\n";
}
int main(int argc,char*argv[])
{
    if(argc<2)
    {
        Usage(argv[0]);
        exit(USER_ERR);
    }
    string ip= argv[1];
    uint16_t port=atoi(argv[2]);
    unique_ptr<udpclient> usev(new udpclient(ip,port));
    usev->init();
    usev->run();
    return 0;
}