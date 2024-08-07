#include "TCPServer.hh"
#include "Unit.hh"
#include <memory>

void fun(Request req, Response& resp)
{
    resp._exitcode = 0;
    resp._calnum = 0;
    switch (req._op)
    {
    case '+':
        resp._calnum = req._x + req._y;
        break;
    case '-':
        resp._calnum = req._x - req._y;
        break;
    case '*':
        resp._calnum = req._x * req._y;
        break;
    case '/':
        if (req._y == 0)
        {
            resp._exitcode = -1;
        }
        else
            resp._calnum = req._x / req._y;
        break;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "\nUsage:\n\t" << argv[0] << "loacl_port" << endl;
        exit(0);
    }
    uint16_t port = stoi(argv[1]);
    unique_ptr<TCPServer> ts(new TCPServer(port, fun));
    ts->init();
    ts->run();
}