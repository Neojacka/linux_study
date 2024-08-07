#include"Ringqueue.hpp"
#include"task.hh"
#include<cstdlib>
#include<ctime>
#include<string>
#include<unistd.h>
void* product(void* agc)
{
     Ringqueue<cultask<int>>* rq=static_cast< Ringqueue<cultask<int>>*>(agc);
     while(true)
     {
        string _ops="=-/*%";
        int x=rand()%100;
        int y=rand()%50;
        char op=_ops[rand()%5];
        cultask<int> t(x,y,op,mymath);
        rq->push(t);
        cout<<"发布任务："<<t.toTask()<<endl;
     }
}
void* custmer(void* agc)
{
    Ringqueue<cultask<int>>* rq=static_cast< Ringqueue<cultask<int>>*>(agc);
    while(true)
    {
        cultask<int> T;
        rq->pop(&T);
        cout<<"完成任务:"<<T()<<endl;
        sleep(1);
    }
}
int main()
{
    srand((unsigned int)time(nullptr));
    Ringqueue<cultask<int>>* rq=new Ringqueue<cultask<int>>();
    pthread_t c,p;
    pthread_create(&c,nullptr,custmer,rq);
    pthread_create(&p,nullptr,product,rq);

    pthread_join(c,nullptr);
    pthread_join(p,nullptr);
    delete rq;
    return 0;
}
