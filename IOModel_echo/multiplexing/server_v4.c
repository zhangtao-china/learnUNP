#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <stdlib.h>
#include <poll.h>
#include <limits.h>
#include <sys/resource.h>
#include <sys/stropts.h>
#include "../../base/wrapsocket.h"
#include "../../base/error.h"
#include "../../base/commondef.h"
#include "../../base/wrapio.h"
#include "../../base/wrapunix.h"

#ifndef INFTIM
    #define INFTIM (-1)
#endif

int main(int argc, char **argv)
{
#ifndef OPEN_MAX
    struct rlimit limit;
    if(getrlimit(RLIMIT_NOFILE, &limit) < 0)
    {
        err_sys("getrlimit failed.");
    }

    const size_t max_open = limit.rlim_cur;
#else
    const size_t max_open = OPEN_MAX;
#endif

    int i, maxi, maxfd, listenfd, connfd, sockfd;
    int nready;
    ssize_t n;
    char buf[MAXLINE];
    socklen_t clilen;
    struct pollfd client[max_open];
    struct sockaddr_in cliaddr, servaddr;
    char clistr[SOCKADDR_STR_BUF_LEN];

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;
    for(i = 1; i < max_open; i++)
    {
        client[i].fd = -1;
    }
    maxi = 0;

    for(;;)
    {
        nready = Poll(client, maxi + 1, INFTIM);

        if(client[0].revents & POLLRDNORM)
        {
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd, (sockaddr*)&cliaddr, &clilen);

            if(sock_ntop((sockaddr *)&cliaddr, clilen, clistr, sizeof(clistr)) != NULL)
            {
                printf("[server_v4] accepted client %s.\n", clistr);
            }

            for(i = 1; i < max_open; i++)
            {
                if(client[i].fd < 0)
                {
                    client[i].fd = connfd;
                    break;
                }
            }

            if(i == max_open)
            {
                err_quit("[server_v4] too many clients.");
            }

            client[i].events = POLLRDNORM;
            if(i > maxi)
            {  maxi = i;  }

            if(--nready <= 0)
            {  continue;  }
        }
        
        for(i = 1; i <= maxi; i++)
        {
            if((sockfd = client[i].fd) < 0)
            {  continue;  }

            if(client[i].revents & (POLLRDNORM | POLLERR))
            {
                if((n = read(sockfd, buf, MAXLINE)) < 0)
                {
                    if(errno == ECONNRESET)
                    {
                        clilen = sizeof(cliaddr);
                        Getpeername(sockfd, (sockaddr *)&cliaddr, &clilen);
                        if(sock_ntop((sockaddr *)&cliaddr, clilen, clistr, sizeof(clistr)) != NULL)
                        {
                            printf("[server_v4] connection %s reset by client.\n", clistr);
                        }

                        Close(sockfd);
                        client[i].fd = -1;
                    }
                    else
                    {
                        err_sys("[server_v4] read error");
                    }
                }
                else if(n == 0)
                {
                    clilen = sizeof(cliaddr);
                    Getpeername(sockfd, (sockaddr *)&cliaddr, &clilen);
                    if(sock_ntop((sockaddr *)&cliaddr, clilen, clistr, sizeof(clistr)) != NULL)
                    {
                        printf("[server_v4] connection %s closed by client.\n", clistr);
                    }

                    Close(sockfd);
                    client[i].fd = -1;
                }
                else
                {
                    Writen(sockfd, buf, n);
                }

                if(--nready <= 0)
                {
                    break;
                }
            }
        }
    }
}

