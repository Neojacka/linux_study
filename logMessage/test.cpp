#include"logMessage.hh"

int main()
{
    logMessage(NORMAL,"创建%s成功","sockfd");
    logMessage(ERROR,"%s失败","Bind");
    return 0;
}