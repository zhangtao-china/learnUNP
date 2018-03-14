#include <stdio.h>
#include "../base/wrapsocket.h"
#include "../base/error.h"
#include "../base/wrapio.h"


int main(int argc, char **argv)
{
    int sockfd;

    if(argc != 3)
    {
        err_quit("usage: tcpsend04 <host> <port#>");
    }

    sockfd = tcp_connect(argv[1], argv[2]);

    Write(sockfd, "123", 3);
    printf("wrote 3 bytes of normal data\n");

    Send(sockfd, "4", 1, MSG_OOB);
    // Send(sockfd, "4xyz", 4, MSG_OOB);
    printf("wrote 1 byte of OOB data\n");

    Write(sockfd, "5", 1);
    printf("wrote 1 byte of normal data\n");

    return 0;
}