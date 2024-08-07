#include"PthreadPool.hpp"

int main()
{
    PthreadPool<cultask<int>>* pp=new PthreadPool<cultask<int>>();
    pp->run();
    while(true)
    {
        int x,y;
        char op;
        cout<<"请输入数据1#";
        cin>>x;
        cout<<"请输入数据2#";
        cin>>y;
        cout<<"请输入操作符#";
        cin>>op;
        cultask<int> t(x,y,op,mymath);
        pp->push(t);
        cout<<"发布任务："<<t.toTask()<<endl;
        sleep(1);
    }
    delete pp;
    return 0;
}