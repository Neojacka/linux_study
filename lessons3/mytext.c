#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
int main()
{
    pid_t id=fork();
    if(id==0)
    {
        int cnt=50;
        while(cnt)
        {
            printf("我是子进程:%d,cnt=%d\n",getpid(),cnt--);
            sleep(1);
        }
    }
    while(1)
    {
        printf("我是父进程:%d\n");
        sleep(1);
    }
    return 0;
}
