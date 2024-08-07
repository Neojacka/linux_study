#pragma once
#include<iostream>
#include<string>
#include<jsoncpp/json/json.h>
#define LINE_SEP "\r\n"
#define LINE_SEP_LEN strlen(LINE_SEP)

using namespace std;

//形成报文
string enLength(const string& text)
{
    string send_str = to_string(text.size());
    send_str += LINE_SEP;
    send_str += text;
    send_str += LINE_SEP;
    return send_str;
}

//解析报文
bool delLength(string& package,string*text)
{
    auto pos=package.find(LINE_SEP);
    if(pos==string::npos) return false;
    int text_len=stoi(package.substr(0,pos));
    *text=package.substr(pos+LINE_SEP_LEN,text_len);
    return true;
}
bool recvrequest(int sock, string* text, string& inbuff)
{
    char buff[1024];
    while (true)
    {
        ssize_t n = read(sock, buff, sizeof(buff) - 1);
        if (n > 0)
        {
            buff[n] = 0; // 确保字符串以 '\0' 结尾
            inbuff += buff;
            // 开始处理
            auto pos = inbuff.find(LINE_SEP);
            if (pos == string::npos) continue;
            string text_len = inbuff.substr(0, pos);
            int len = stoi(text_len);
            int entext_len = len + 2 * LINE_SEP_LEN + text_len.size();
            if (inbuff.size() < entext_len) continue;
            *text = inbuff.substr(0, entext_len);
            inbuff.erase(0, entext_len);
            break;
        }
        else
            return false;
    }
    return true;
}


class Request{
public:
    Request(){}
    Request(int x,int y,char op)
        :_x(x),_y(y),_op(op)
    {}
    bool Serialize(string* out)
    {
        Json::Value root;
        root["first"]=_x;
        root["second"]=_y;
        root["oper"]=_op;
        Json::FastWriter writer;
        *out=writer.write(root);
        return true;
    }
    bool Unserialize(string& in)
    {
        Json::Value root;
        Json::Reader reader;
        reader.parse(in,root);
        _x=root["first"].asInt();
        _y=root["second"].asInt();
        _op=root["oper"].asInt();
        return true;
    }
public:
    int _x;
    int _y;
    char _op;
};
class Response{
public:
    Response(){}
    Response(int exitcode,int calnum)
        :_exitcode(exitcode),_calnum(calnum)
    {}
    bool Serialize(string* out)
    {
        Json::Value root;
        root["code"]=_exitcode;
        root["num"]=_calnum;
        Json::FastWriter writer;
        *out=writer.write(root);
        return true;
    }
    bool  Unserialize(string& in)
    {
        Json::Value root;
        Json::Reader reader;
        reader.parse(in,root);
        _exitcode=root["code"].asInt();
        _calnum=root["num"].asInt();
        return true;
    }
public:
    int _exitcode;
    int _calnum;
};