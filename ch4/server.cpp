#include <iostream>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <ctime>
#include <cstring>

int main(int argc, char **argv)
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0)
    {
        std::cerr << "socket error" << std::endl;
    }

    sockaddr_in servaddr;
    ::bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = ::htons(8899);
    servaddr.sin_addr.s_addr = ::htonl(INADDR_ANY);

    if(bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        std::cout << "bind error" << std::endl;
        return -1;
    }

    if(listen(listenfd, 5) < 0)
    {
        std::cout << "listen error" << std::endl;
        return -1;
    }

    std::cout << "------------after listen" << std::endl;
    char buf[128];
    for(;;)
    {
        int connfd = accept(listenfd, (sockaddr *)NULL, NULL);

        time_t ticks;
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        write(connfd, buf, strlen(buf));
    }

    return 0;
}