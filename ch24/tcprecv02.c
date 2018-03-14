#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "../base/error.h"
#include "../base/wrapunix.h"


int main(int argc, char **argv)
{
    int listenfd, connfd, n;
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
        FD_SET(connfd, &xset);

/*
select一直指示一个异常条件，直到进程读入越过带外数据。同一个数据不能读入多次，因为首次读入之后，
内核就清空这个单字节的缓冲区。再次指定MSG_OOB标志调用recv时，它将返回EINVAL
*/
        Select(connfd + 1, &rset, NULL, &xset, NULL);

        if(FD_ISSET(connfd, &xset))
        {
            printf("-----------------------------------------------\n");
            n = Recv(connfd, buff, sizeof(buff) - 1, MSG_OOB);
            buff[n] = 0;
            printf("read %d OOB byte: %s\n", n, buff);
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
        }
    }

    return 0;
}