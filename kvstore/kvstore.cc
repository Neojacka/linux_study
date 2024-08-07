#include "kvstore.hh"
#include <memory>
#include"string"
unordered_map<string, function<string(vector<string> &)>> _task;
map<string, string> _data; // 数据存储
string SET(vector<string> &texts)
{
    if (texts.size() == 3)
    {
        if (_data.insert(make_pair(texts[1], texts[2])).second)
            return "SET successful";
    }
    return "SET false";
}
string GET(vector<string> &texts)
{
    if (texts.size() == 2)
    {
        auto pos = _data.find(texts[1]);
        if (pos != _data.end())
            return _data[texts[1]];
    }
    return "GET false";
}
string DEL(vector<string> &texts)
{
    if (texts.size() == 2)
    {
        auto pos = _data.find(texts[1]);
        if (pos != _data.end())
            _data.erase(texts[1]);
        return "DEL successful";
    }
    return "DEL false";
}
string MOD(vector<string> &texts)
{
    if (texts.size() == 3)
    {
        string key = texts[1];
        string value = texts[2];
        auto pos = _data.find(key);
        if (pos != _data.end())
        {
            _data[key] = value;
            return "MOD successful";
        }
    }
    return "MOD false";
}
void addfun()
{

    _task.insert(make_pair("SET", SET));
    _task.insert(make_pair("GET", GET));
    _task.insert(make_pair("DEL", DEL));
    _task.insert(make_pair("MOD", MOD));
}
void service(Connection *conn, string &message)
{
    stringstream ss(message);
    string text;
    vector<string> texts;
    while (ss >> text)
    {
        texts.push_back(text);
#ifdef DEBUG
        cout << text << " ";
#endif
    }
    auto it = _task.find(texts[0]);
    if (it != _task.end())
    {
        conn->_outbuff = it->second(texts);
    }
    else
    {
        conn->_outbuff = "Unknown command";
    }

    conn->_sender(conn);
    
    
}
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "\nUser\n\t" << argv[0] << "Serverip Serverport" << endl;
        exit(1);
    }
    addfun();
    unique_ptr<TCPServer> ts(new TCPServer(stoi(argv[1]), service));
    ts->init();
    ts->Dispatcher();
    return 0;
}