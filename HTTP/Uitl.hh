#pragma once
#include<iostream>
#include<string>
#include<cstring>
#include<sstream>
#include <sys/stat.h>
#include<fstream>
#include<unordered_map>
#define LINE_SET "\r\n"
#define default_root "wwwroot"
#define home_page "index.html"
using namespace std;


class Util {
public:
    static string getheadline(string& buff) {
        auto pos = buff.find(LINE_SET);
        if (pos == string::npos) return "";
        string headline = buff.substr(0, pos);
        buff.erase(0, headline.size() + strlen(LINE_SET));
        cout << "成功获取请求行" << endl;
        return headline;
    }
    static bool readfile(const string resoucer,string *out)
    {
        ifstream in(resoucer);
        if(!in.is_open())
        {
            return false;
        }
        string line;
        while(getline(in,line))
        {
            *out+=line;
        }
        in.close();
        return true;
    }
};

class Request {
public:
    void parse() {
        // 1. 获取请求行消息
        string reqline = Util::getheadline(inbuff);
        cout << "DEBUG: 请求行: " << reqline << endl;
        stringstream ss(reqline);
        ss >> method >> url >> httpversion;

         // 2.1 /search?name=zhangsan&pwd=12345
        //通过?将左右进行分离
        //如果是POST方法，本来就是分离的!
        //左边PATH，右边parm
        auto pos1=url.find('?');
        if(pos1==string::npos)
            parm="";
        else
        {
            parm=url.substr(pos1+1);
            url=url.substr(pos1);  
        }
        // 2.2 添加web默认路径
        path = default_root;
        path += url; // wwwroot/url
        if (path[path.size() - 1] == '/')
            path += home_page;
        // 3. 获取path路径请求资源后缀
        //wwwroot/index.html
        //wwwroot/test/1.jpg
        //wwwroot/home/2.css
        auto pos = path.rfind('.');
        if (pos == string::npos) 
            suffix = ".html";//给一个默认
        else
            suffix = path.substr(pos);

        // 4. 得到请求资源大小
        struct stat st;
        int n = stat(path.c_str(), &st);
        if (n == 0)
            size = st.st_size;
        else
            size = -1; 

    }
public:
    string method;//请求方法
    string path;//web路径
    string url;//请求路径
    string httpversion;//http版本
    string suffix;//资源后缀，请求什么资源
    string inbuff;
    int size;//请求资源大小
    string parm;//参数
};


class Response{

public:
    string outbuff;
};