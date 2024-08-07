#pragma once
#include <iostream>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <jsoncpp/json/json.h>
#define LINE_SEP "\r\n"
#define LINE_SEP_LEN strlen(LINE_SEP)
using namespace std;
class Util
{
public:
    // 将文件描述符设置为非阻塞
    static void SetNonBlack(int fd)
    {
        int fl = fcntl(fd, F_GETFL);
        fcntl(fd, F_SETFL, fl | O_NONBLOCK);
    }

    static void PackageSplit(string &inbuff, vector<string> &result)
    {
        // 报文协议："content_len"\r\n"x op y"\r\n
        while (true)
        {
            auto pos = inbuff.find(LINE_SEP);
            if (pos == string::npos)
                break;
            else
            {
                string text_len_string = inbuff.substr(0, pos);
                int text_len = stoi(text_len_string);
                int total_len = text_len_string.size() + 2 * LINE_SEP_LEN + text_len;
                if (inbuff.size() < total_len)
                    continue;
                else
                {
                    // 得到正文
                    result.push_back(inbuff.substr(pos + LINE_SEP_LEN, text_len));
                    inbuff.erase(0, total_len);
                }
            }
        }
    }
};
class Request
{
public:
    Request() {}
    Request(int x, char op, int y)
        : _x(x), _y(y), _op(op)
    {
    }
    bool Serialize(string *out)
    {
        Json::Value root;
        root["first"] = _x;
        root["second"] = _op;
        root["third"] = _y;
        Json::FastWriter writer;
        *out = writer.write(root);
        return true;
    }
    bool Unserialize(string &in)
    {
        Json::Value root;
        Json::Reader reader;
        reader.parse(in, root);
        _x = root["first"].asInt();
        _op = root["second"].asInt();
        _y = root["third"].asInt();
        return true;
    }

public:
    int _x;
    int _y;
    char _op;
};
class Response
{
public:
    Response() {}
    Response(int exitcode, int calnum)
        : _exitcode(exitcode), _calnum(calnum)
    {
    }
    bool Serialize(string *out)
    {
        Json::Value root;
        root["code"] = _exitcode;
        root["num"] = _calnum;
        Json::FastWriter writer;
        *out = writer.write(root);
        return true;
    }
    bool Unserialize(string &in)
    {
        Json::Value root;
        Json::Reader reader;
        reader.parse(in, root);
        _exitcode = root["code"].asInt();
        _calnum = root["num"].asInt();
        return true;
    }

public:
    int _exitcode;
    int _calnum;
};
// 形成报文
string enLength(const string &text)
{
    string send_str = to_string(text.size());
    send_str += LINE_SEP;
    send_str += text;
    send_str += LINE_SEP;
    return send_str;
}
// 解析报文
bool delLength(string &package, string *text)
{
    auto pos = package.find(LINE_SEP);
    if (pos == string::npos)
        return false;
    int text_len = stoi(package.substr(0, pos));
    *text = package.substr(pos + LINE_SEP_LEN, text_len);
    return true;
}
bool recvrequest(int sock, string *text, string &inbuff)
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
            if (pos == string::npos)
                continue;
            string text_len = inbuff.substr(0, pos);
            int len = stoi(text_len);
            int entext_len = len + 2 * LINE_SEP_LEN + text_len.size();
            if (inbuff.size() < entext_len)
                continue;
            *text = inbuff.substr(0, entext_len);
            inbuff.erase(0, entext_len);
            break;
        }
        else
            return false;
    }
    return true;
}
