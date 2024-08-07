#include<iostream>
#include<string>
#include<vector>
#include<cstdio>
#include<unistd.h>
#include<cstdlib>
#include<cassert>
#include<time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <sys/wait.h>
#include<fcntl.h>
using namespace std;
typedef void(*Fun_t)();
void Fun1()
{
    cout<<"Fun0"<<endl;
    sleep(1);
}
void Fun2()
{
    cout<<"Fun1"<<endl;
    sleep(1);
}
void Fun3()
{
    cout<<"Fun2"<<endl;
    sleep(1);
}
void Fun4()
{
    cout<<"Fun3"<<endl;
    sleep(1);
}
void loadtask(vector<Fun_t> &Map)
{
    Map.push_back(Fun1);
    Map.push_back(Fun2);
    Map.push_back(Fun3);
    Map.push_back(Fun4);
}
class subp
{
    public:
    subp(const int &writefd,const pid_t id)
    :_writefd(writefd),_id(id)
    {
        char name[1024];
        sprintf(name,"process:%d [pid(%d)-fd(%d)]",cnt++,_id,_writefd);
        _name=name;
    }
public:
    int _writefd;
    pid_t _id;
    static int cnt;
    string _name;
};
int subp::cnt=0;
void Sendtask(const subp &sub,int taskid)
{
    cout<<"父进程"<<getpid()<<"给子进程"<<sub._name<<"发送任务"<<taskid<<endl;
    ssize_t n=write(sub._writefd,&taskid,sizeof(int));
    assert(n==sizeof(int));
}

int gettask(int readfd)
{
    
    int code=0;
    ssize_t n =read(readfd,&code,sizeof(code));
    if(n==sizeof(code))
        return code;
    else if(n<=0)
        return -1;
    else
        return 0;
 
}
void createsubprocess(vector<subp> &out,const vector<Fun_t> &Funmap)
{
    vector<int> deletefd;
    for(int i=0; i<5; i++)
    {
        
        int fds[2];
        int n = pipe(fds);
        assert(n==0);
        pid_t id =fork();
        if(id<0)
            {
                cout<<"进程创建失败"<<endl;
                exit(1);
            }
        else if(id==0)
        {
            //除第一次创建子进程，其他次创建子进程都会继承上子进程的writefd
            for(int j=0;j<deletefd.size();j++)
            {
                close(deletefd[j]);
            }
            close(fds[1]);
            //此时处于阻塞状态，因为管道没有东西，即父进程没有写
            while(true)
            {
                //获取命令码
                int code=gettask(fds[0]);
                //执行命令
                if (code > 0 && code < Funmap.size())
                {
                    cout<<"子进程"<<getpid()<<"执行";
                    Funmap[code]();
                }   
                else if(code==-1)
                    break;
            }
            exit(0);
        }
        close(fds[0]);
        subp sub(fds[1],id);
        out.push_back(sub);
        deletefd.push_back(fds[1]);
    }
}
void contrlprocess(const vector<subp> &Sub,const vector<Fun_t> &FunMap)
{
    int processnum=Sub.size();
    int Fun_tnum=FunMap.size();
    bool quit=false;
    while(!quit)
    {
        //选择一个子进程
        int subid=rand()%processnum;
        //选择一个任务
        int taskid=rand()%Fun_tnum;
        Sendtask(Sub[subid],taskid);
        sleep(1);
    }
}
int main()
{
    srand((unsigned int)time(NULL));
   //加载任务
    vector<Fun_t> FunMap;
    loadtask(FunMap);
    //创建子进程
    vector<subp> Sub;
    createsubprocess(Sub,FunMap);
    //父进程控制子进程
    contrlprocess(Sub,FunMap);
    // 让父进程 wait 所有子进程
    for(int i=0; i<Sub.size(); ++i)
    {
        waitpid(Sub[i]._id, NULL, 0);
        cout<<"wait subprocess success:"<<Sub[i]._id<<endl;
    }
    return 0;
}