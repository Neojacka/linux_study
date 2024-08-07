#include "Socket.hpp"
#include <iostream>
#include <thread>
#include <vector>
void connection(string serveip, uint16_t port)
{
    thread::id thread_id = this_thread::get_id();
    int _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd < 0)
    {
        cerr << "SOCKET ERRNO " << errno << strerror(errno) << endl;
    }
    // 设置端口复用，避免timewait阻塞端口
    int opt = 1;
    setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    struct sockaddr_in clocal;
    bzero(&clocal, sizeof(clocal));
    clocal.sin_addr.s_addr = inet_addr(serveip.c_str());
    clocal.sin_family = AF_INET;
    clocal.sin_port = htons(port);

    int n = connect(_sockfd, (struct sockaddr *)&clocal, sizeof(clocal));
    if (n < 0)
    {
        cerr << "Connect ERRNO " << errno << strerror(errno) << endl;
    }
    int i = 0;
    while (i++ < 500)
    {
        char buff[1024];
        snprintf(buff, sizeof(buff), "SET %d %d", i, thread_id);
        send(_sockfd, buff, sizeof(buff), 0);
    }
    close(_sockfd);
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "\nUser\n\t" << argv[0] << "Serverip Serverport" << endl;
        exit(1);
    }
    vector<thread> threads;
    uint16_t port = stoi(argv[2]);
    time_t start = time(nullptr);
    for (int i = 0; i < 5; i++)
    {
        threads.emplace_back(connection, argv[1], port);
    }
    for (auto &th : threads)
    {
        if (th.joinable())
        {
            th.join();
        }
    }
    time_t end = time(nullptr);
    cout << end - start << endl;
    return 0;
}