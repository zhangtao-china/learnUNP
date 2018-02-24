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
    int sockfd, res, connfd, flags;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        err_exit("socket");
    }

    flags = fcntl(sockfd, F_GETFL, 0);
    if(flags < 0)
    {
        err_exit("fcntl F_GETFL");
    }

    res = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    if(res < 0)
    {
        err_exit("fcntl F_SETFL");
    }
    printf("listen fd: nonblocking mode\n");

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9977);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        err_exit("bind");
    }

    if(listen(sockfd, 5) < 0)
    {
        err_exit("listen");
    }

    fd_set rset;
    bzero(&rset, sizeof(rset));
    FD_SET(sockfd, &rset);
    for(;;)
    {
        res = select(sockfd + 1, &rset, NULL, NULL, NULL);
        if(res < 0)
        {
            err_exit("select");
        }

        if((connfd = accept(sockfd, NULL, NULL)) < 0)
        {
            if(EWOULDBLOCK == errno)
            {
                printf("accept error: EWOULDBLOCK\n");
                continue;
            }
            err_exit("accept");
        }
        else
        {
            flags = fcntl(connfd, F_GETFL, 0);
            if(flags < 0)
            {
                err_exit("fcntl");
            }
            else
            {
                /*
                 * 监听套接字是非阻塞模式时，连接套接字模式还是阻塞模式
                */
                int f = O_NONBLOCK;
                if((flags & f) == f && (f != 0 || flags == f))
                {
                    printf("connfd is nonblocking I/O\n");
                }
                else
                {
                    printf("connfd is blocking I/O\n");
                }

                exit(0);
            }
        }
    }
}

