#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<sys/wait.h>
#define trimspace(start)  do{\
                         while(isspace(*start)) start++;\
                    }while(0)
#define NONE_REDIR 0
#define INPUT_REDIR 1
#define OUTPUT_REDIR 2
#define APPEND_REDIR 3
#define option 64
#define Max 100
char linecommand[Max];
char *myargv[option];
char* file=NULL;
int lastcode;
int redirtype=NONE_REDIR;
void getfile(char* linecommand)
{
    char*start=linecommand;
    char* end=linecommand+strlen(linecommand);
    while(start<end)
    {
        if(*start=='<')
        {
            *start='\0';
            start++;
            trimspace(start);
            redirtype=INPUT_REDIR;
            file=start;
            break;
        }
        else if(*start=='>')
        {
            *start='\0';
            start++;
            if(*start=='>')
            {
                start++;
                redirtype=APPEND_REDIR;
            }
            else
            {
                redirtype=OUTPUT_REDIR;
            }
            trimspace(start);
            file=start;
            break;
        }
        else
            start++;
    }
}
int main()
{
    while(1)
    {
        printf("[%s@%s]$ ",getenv("USER"),getenv("PWD"));
        fflush(stdout);
        //获取输入指令
        fgets(linecommand,sizeof(linecommand)-1,stdin);
        //去除输入指令敲回车产生的\n
        linecommand[strlen(linecommand)-1]=0;
        //重定向分离出文件
        getfile(linecommand);
        //切割字符串，转成指令
        myargv[0]=strtok(linecommand," ");
        int i=1;
        //给ls命令添加颜色
        if(strcmp(myargv[0],"ls")==0)
        {
            myargv[i++]=(char*)"--color=auto";
        }
        //如果是内建/内置命令(父进程执行的命令)，不需要子进程来执行，shell自己执行，其本质是执行系统接口
        while(myargv[i++]=strtok(NULL," "));
        if(strcmp(myargv[0],"cd")==0&& myargv[1] !=NULL)
        {
            //chdir()，修改工作路径
            chdir(myargv[1]);//下面子进程执行的命令不会影响父进程，子进程的路径是父进程的,要使用cd命令，必须修改父进程的路径
            continue;
        }
        if(myargv[0]!=NULL && myargv[1]!=NULL &&strcmp(myargv[0],"echo")==0)
        {
            if(strcmp(myargv[1],"$?")==0)
            {
                printf("%d",lastcode);
            }
            else 
            {
                printf("%s",myargv[1]);
            }
            continue;//跳过下面剩余代码，重新循环
        }
        pid_t id = fork();
if (id == 0) {
    int fd;
    if (redirtype != NONE_REDIR && file != NULL) 
    {
        if (redirtype == INPUT_REDIR) 
        {
            fd = open(file, O_RDONLY);
        } 
        else 
        {
            int flags = O_WRONLY | O_CREAT;
            if (redirtype == APPEND_REDIR) 
            {

                flags |= O_APPEND;
            } else 
            {
                flags |= O_TRUNC;
            }
            fd = open(file, flags, 0666);
        }
        if (fd < 0) 
        {
            perror("open");
            exit(2);
        }
        dup2(fd, redirtype == INPUT_REDIR ? 0 : 1);
        close(fd);
    }

    if (execvp(myargv[0], myargv) < 0) 
    {
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    
    exit(1);
}

        int status=0;
        int ret = waitpid(id,&status,0);
        lastcode=(status>>8)&0xff;
        
    }
}
