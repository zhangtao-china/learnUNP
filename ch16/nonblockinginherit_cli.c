#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

static void err_exit(const char *info)
{
    printf("%s error: %s\n", info, strerror(errno));
    exit(-1);
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("usage: nonblockinginherit_cli.c IPAddress\n");
        return -1;
    }

    int sockfd, res;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        err_exit("socket");
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9977);
    res = inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    if(res < 0)
    {
        err_exit("inet_pton");
    }
    else if(res == 0)
    {
        printf("invalid IP address\n");
        return -1;
    }

    res = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if(res < 0)
    {
        err_exit("connect");
    }

    sleep(5);

    close(sockfd);

    return 0;
}