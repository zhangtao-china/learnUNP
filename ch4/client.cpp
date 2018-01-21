#include <iostream>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cerr << "usage: a.out <IPaddress>" << std::endl;
        return -1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
    }

    sockaddr_in servaddr;
    ::bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = ::htons(8899);
    if(::inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
    {
        std::cerr << "inet_pton error for " << argv[1] << std::endl;
        return -1;
    }

    if(::connect(sockfd, (sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        std::cerr << "connect error" << std::endl;
        return -1;
    } 

    std::cout << "----------------connected" << std::endl;

    char buf[1024] = {};
    int n = 0;
    while((n = ::read(sockfd, buf, 1024)) > 0)
    {
        buf[n] = 0;
    }

    if(n < 0)
    {
        std::cerr << "read error" << std::endl;
        return -1;
    }

    return 0;
}