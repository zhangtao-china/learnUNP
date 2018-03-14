#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "../base/error.h"
#include "../base/wrapunix.h"


int main(int argc, char **argv)
{
    int listenfd, connfd, n, justreadoob = 0;
    char buff[128];
    fd_set rset, xset;

    if(argc == 2)
    {
        listenfd = tcp_listen(NULL, argv[1], NULL);
    }
    else if(argc == 3)
    {
        listenfd = tcp_listen(argv[1], argv[2], NULL);
    }
    else
    {
        err_quit("usage: tcprecv02 [ <host> ] <port#>");
    }

    connfd = Accept(listenfd, NULL, NULL);

    FD_ZERO(&rset);
    FD_ZERO(&xset);

    for(;;)
    {
        FD_SET(connfd, &rset);

        if(justreadoob == 0)
        {
            FD_SET(connfd, &xset);
        }

        Select(connfd + 1, &rset, NULL, &xset, NULL);

        if(FD_ISSET(connfd, &xset))
        {
            printf("-----------------------------------------------\n");
            n = Recv(connfd, buff, sizeof(buff) - 1, MSG_OOB);
            buff[n] = 0;
            printf("read %d OOB byte: %s\n", n, buff);
            justreadoob = 1;
            FD_CLR(connfd, &xset);
            printf("-----------------------------------------------\n");
        }

        if(FD_ISSET(connfd, &rset))
        {
            if((n = Read(connfd, buff, sizeof(buff) - 1)) == 0)
            {
                printf("received EOF\n");
                return 0;
            }

            buff[n] = 0;
            printf("read %d bytes: %s\n", n, buff);
            justreadoob = 0;
        }
    }

    return 0;
}