#include "TCPServer.hpp"
#include <memory>

void handler(Request &req, Response &resp)
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
void service(Connection *conn, string &text)
{
    Request req;
    Response res;
    cout<<text<<endl;
    if (req.Unserialize(text))
    {
        cout<<req._x<<req._op<<req._y<<endl;
        handler(req, res);
    }
    string message;
    res.Serialize(&message);
    string sendme=enLength(message);
    conn->_outbuff += sendme;
    cout<<conn->_outbuff<<endl;
    conn->_sender(conn);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "\nUser\n\t" << argv[0] << "Serverip Serverport" << endl;
        exit(1);
    }
    unique_ptr<TCPServer> ts(new TCPServer(stoi(argv[1]), service));
    ts->init();
    ts->Dispatcher();
    return 0;
}