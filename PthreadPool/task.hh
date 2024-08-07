#include<iostream>
#include<string>
#include<functional>

using namespace std;
int mymath(int& x,int& y,char op)
{
    int result;
    switch(op)
    {
        case '+':
             result=x+y;
             break;
        case '-':
            result=x-y;
            break;
        case '*':
            result=x*y;
            break;
        case '/':
        {
            if(y==0)
            {
                cerr<<"div zero error"<<endl;
                return -1;
            }
            else
                result= x/y;
                break;
        }
        case '%':
        {
            if(y==0)
            {
                cerr<<"mod zero error"<<endl;
                return -1;
            }
            else
                result=x%y;
                break;
        }
        default:
            break;
    }
    return result;
}
template<class T>
class cultask 
{
    using fun_t = function<int(int&, int&, char)>;
public:
    cultask()
    {}
    cultask(const T& x, const T& y, char op, fun_t fun)
        : _x(x), _y(y), _op(op), _callback(fun) {}

    // 消费任务
    string operator()() 
    {
        int result = _callback(_x, _y, _op);
        char buff[1024];
        snprintf(buff, sizeof(buff), "%d %c %d = %d", _x, _op, _y, result);
        return buff;
    }

    // 发送任务
    string toTask() {
        char buff[1024];
        snprintf(buff, sizeof(buff), "%d %c %d = ?", _x, _op, _y);
        return buff;
    }

private:
    T _x, _y;
    char _op;
    fun_t _callback;
};



class singtask
{
    using fun_t=function<void(string&)>;
public:
    singtask()
    {}
    singtask(const string& messeage,fun_t fun)
        : _messeage(messeage),_fun(fun)
    {}
    void operator()()
    {
        _fun(_messeage);
    }
private:
    string _messeage;
    fun_t _fun;
};
void save(string& messages)
{
    string targe="./log.txt";
    FILE* fp=fopen(targe.c_str(),"a+");
    if(!fp)
    {
        perror("fopen");
        return;
    }
    fputs(messages.c_str(),fp);
    fclose(fp);
}