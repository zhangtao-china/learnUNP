#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <stdlib.h>
#include "../../base/wrapsocket.h"
#include "../../base/error.h"
#include "../../base/commondef.h"
#include "../../base/wrapio.h"
#include "../../base/wrapunix.h"


int main(int argc, char **argv)
{
    int i, maxi, maxfd, listenfd, connfd, sockfd;
    int nready, client[FD_SETSIZE];
    ssize_t n;
    fd_set rset, allset;
    char buf[MAXLINE];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char clistr[INET_ADDRSTRLEN];

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    maxfd = listenfd;
    maxi = -1;
    for(i = 0; i < FD_SETSIZE; i++)
    {
        client[i] = -1;
    }
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    for(;;)
    {
        rset = allset;
        nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);

        if(FD_ISSET(listenfd, &rset))
        {
            /*
            * 存在的问题:
            * 当客户设置了SO_LINGER套接字选项,把l_onoff标志设置为1,把l_linger时间设置为0.
            * 这样的设置导致连接被关闭时在TCP套接字上发送一个RST. 假设服务器非常繁忙,当连接就绪后没有马上调用accept,
            * 而在此期间客户关闭了套接字,服务器接收到RST后将链接驱除出队列,假设队列中已没有其他已完成连接,随后服务器调用accept,
            * 这将导致服务器一直阻塞在accept上,知道其他某个客户建立一个连接为止, 在此期间服务无法处理其他客户的请求.
            *
            */
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd, (sockaddr*)&cliaddr, &clilen);

            if(Inet_ntop(AF_INET, &cliaddr.sin_addr, clistr, sizeof(clistr)) != NULL)
            {
                printf("[server_v3] accepted client %s:%hu.\n", clistr, cliaddr.sin_port);
            }

            for(i = 0; i < FD_SETSIZE; i++)
            {
                if(client[i] < 0)
                {
                    client[i] = connfd;
                    break;
                }
            }

            if(i == FD_SETSIZE)
            {
                err_quit("[server_v3] too many clients");
            }

            FD_SET(connfd, &allset);
            if(connfd > maxfd)
            {  maxfd = connfd;  }

            if(i > maxi)
            {  maxi = i;  }

            if(--nready <= 0)
            {  continue;  }
        }
        
        for(i = 0; i <= maxi; i++)
        {
            if((sockfd = client[i]) < 0)
            {  continue;  }

            if(FD_ISSET(sockfd, &rset))
            {
                if((n = Read(sockfd, buf, MAXLINE)) == 0)
                {
                    clilen = sizeof(cliaddr);
                    Getpeername(sockfd, (sockaddr *)&cliaddr, &clilen);
                    if(Inet_ntop(AF_INET, &cliaddr.sin_addr, clistr, sizeof(clistr)) != NULL)
                    {
                        printf("[server_v3] connecton %s:%hu closed by client.\n", clistr, cliaddr.sin_port);
                    }

                    Close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
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

