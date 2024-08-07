#include"BlockQueue.hh"
#include"task.hh"
#include<cstdlib>
#include<ctime>
#include<unistd.h>
template<class c,class s>
class BlockQueues
{
public:
    BlockQueue<c>* _cbq;//生产-消费阻塞队列
    BlockQueue<s>* _sbq;//消费-保存阻塞队列
};
char op_[5]={'+','-','*','/','%'};
void* product(void* bqs)
{
    BlockQueue<cultask<int>>* bq=(static_cast<BlockQueues<cultask<int>,singtask>*>(bqs))->_cbq;
    while(true)
    {
        int x= rand()%100;
        int y=rand()%30;
        char op=op_[rand()%5];
        cultask<int> cul(x, y, op, mymath);
        bq->push(cul);
        cout<<"生成任务："<<cul.toTask()<<endl;
        //sleep(1);
    }
    return nullptr;
}
void* consumer(void* bqs)
{
    BlockQueue<cultask<int>>* bq=(static_cast<BlockQueues<cultask<int>,singtask>*>(bqs))->_cbq;
    BlockQueue<singtask>* sbq=(static_cast<BlockQueues<cultask<int>,singtask>*>(bqs))->_sbq;
    while(true)
    {
        cultask<int> T;
        bq->pop(&T);
        string messeage=T();
        cout<<"消费任务:"<<T()<<endl;
        singtask s(messeage,save);
        sbq->push(s);
        cout<<"推送任务完成"<<endl;
        sleep(1);
    }
    return nullptr;
}
void* Save(void*bqs)
{
    BlockQueue<singtask>* sbq=(static_cast<BlockQueues<cultask<int>,singtask>*>(bqs))->_sbq;
    while(true)
    {
        singtask S;
        sbq->pop(&S);
        S();
        cout<<"保存任务成功"<<endl;

    }
    return nullptr;
}
int main()
{
    srand((unsigned int)time(nullptr));
    BlockQueues<cultask<int>,singtask> _bqs;
    _bqs._cbq=new BlockQueue<cultask<int>>();
    _bqs._sbq=new BlockQueue<singtask>();
    pthread_t c,p,s;
    pthread_create(&c,nullptr,consumer,&_bqs);
    pthread_create(&p,nullptr,product,&_bqs);
    pthread_create(&s,nullptr,Save,&_bqs);

    pthread_join(c,nullptr);
    pthread_join(p,nullptr);
    pthread_join(s,nullptr);
    delete _bqs._cbq;
    delete _bqs._sbq;

    return 0;
}