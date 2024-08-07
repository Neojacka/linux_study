#include "HTTPServer.hh"
#include "Uitl.hh"
#include <memory>
#define html_404 "wwwroot/404.html"

string suffixtoDesc(string suffix)
{
    string ct = "Content-type：";
    if (suffix == ".html")
        ct += "text/html";
    else if (suffix == ".jpg")
        ct += "application/x-jpg";
    ct += "\r\n";
    return ct;
}
void fun(Request &req, Response &resp)
{
    cout << "---------http start------------" << endl;
    cout << req.inbuff;
    cout << "method:" << req.method << endl;
    cout << "url:" << req.url << endl;
    cout << "httpversion:" << req.httpversion << endl;
    cout << "path:" << req.path << endl;
    cout << "suffix:" << req.suffix << endl;
    cout << "请求资源大小：" << req.size << endl;
    cout << "---------http end--------------" << endl;

    // 构造完整的HTTP响应
    string respline = "HTTP/1.1 202 OK\r\n";    // 响应行
    string resphead = suffixtoDesc(req.suffix); // 返回什么资源(根据请求的来)
    if (req.size > 0)
    {
        resphead += "Content-Length: "; // 资源长度(根据请求资源的大小)
        resphead += to_string(req.size);
        resphead += "\r\n";
    }
    resphead+="Location: https://www.qq.com/\r\n";//重定向的网站
    string respblank = "\r\n";
    // 回应正文(请求要的资源)
    string body;
    if (!Util::readfile(req.path, &body))
    {
        Util::readfile(html_404, &body); // 一定会成功
    }
    resp.outbuff += respline;
    resp.outbuff += resphead;
    resp.outbuff += respblank;
    
    cout << "---------http start------------" << endl;
    cout << resp.outbuff << endl;
    cout << "---------http end--------------" << endl;

    resp.outbuff += body;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "\nUsage:\n\t" << argv[0] << "loacl_port" << endl;
        exit(0);
    }
    uint16_t port = stoi(argv[1]);
    unique_ptr<HTTPServer> ts(new HTTPServer(port, fun));
    ts->init();
    ts->run();
}