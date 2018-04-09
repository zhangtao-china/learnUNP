#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <stdlib.h>
#include <poll.h>
#include <limits.h>
#include <sys/resource.h>
#include <sys/stropts.h>
#include <sys/epoll.h>
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

    int i, listenfd, connfd, sockfd;
    int nready;
    ssize_t n;
    char buf[MAXLINE];
    socklen_t clilen;
    struct epoll_event ev, events[max_open];
    struct sockaddr_in cliaddr, servaddr;
    char clistr[SOCKADDR_STR_BUF_LEN];

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    int epollfd = epoll_create1(0);
    if(epollfd == -1)
    {
        err_sys("epoll_create");
    }
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1)
    {
        err_sys("epoll_ctl listenfd");
    }

    for(;;)
    {
        nready = epoll_wait(epollfd, events, max_open, -1);

        if(nready == -1)
        {
            err_sys("epoll_wait");
        }

        for(i = 0; i < nready; ++i)
        {
            if((sockfd = events[i].data.fd) == listenfd)
            {
                clilen = sizeof(cliaddr);
                connfd = Accept(listenfd, (sockaddr*)&cliaddr, &clilen);
                if(sock_ntop((sockaddr *)&cliaddr, clilen, clistr, sizeof(clistr)) != NULL)
                {
                    printf("[server_v6] accepted client %s.\n", clistr);
                }

                ev.events = EPOLLIN;
                ev.data.fd = connfd;
                if(epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1)
                {
                    err_sys("epoll_ctl connfd");
                }
            }
            else
            {
                if((n = read(sockfd, buf, MAXLINE)) < 0)
                {
                    if(errno == ECONNRESET)
                    {
                        clilen = sizeof(cliaddr);
                        Getpeername(sockfd, (sockaddr *)&cliaddr, &clilen);
                        if(sock_ntop((sockaddr *)&cliaddr, clilen, clistr, sizeof(clistr)) != NULL)
                        {
                            printf("[server_v6] connection %s reset by client.\n", clistr);
                        }
                        
                        ev.data.fd = sockfd;
                        if(epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, &ev) == -1)
                        {
                            err_sys("epoll_ctl DEL sockfd");
                        }

                        Close(sockfd);
                    }
                    else
                    {
                        err_sys("[server_v6] read error");
                    }
                }
                else if(n == 0)
                {
                    clilen = sizeof(cliaddr);
                    Getpeername(sockfd, (sockaddr *)&cliaddr, &clilen);
                    if(sock_ntop((sockaddr *)&cliaddr, clilen, clistr, sizeof(clistr)) != NULL)
                    {
                        printf("[server_v6] connection %s closed by client.\n", clistr);
                    }

                    
                    ev.data.fd = sockfd;
                    if(epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, &ev) == -1)
                    {
                        err_sys("epoll_ctl DEL sockfd");
                    }

                    Close(sockfd);
                }
                else
                {
                    Writen(sockfd, buf, n);
                }
            }
        }
    }
}

