#include<stdio.h>
#include<unistd.h>
#include <fcntl.h>
int main()
{
    pid_t id=fork();
    if(id==0)
    {
        while(1)
        {
            printf("子进程pid:%d,父进程pid:%d\n",getpid(),getppid());
            sleep(1);
        }
    }
    else if(id>0)
    {
        int cnt=100;
        while(cnt)
        {
            printf("还剩余：%3d s\r",cnt);
            fflush(stdout);
            sleep(1);
            cnt--;
        }
        printf("\n");
    }
    return 0;
}
