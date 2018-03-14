#include <unistd.h>
#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "../base/error.h"


int main(int argc, char **argv)
{
    int listenfd, connfd, n, on = 1;
    char buff[128];

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
        err_quit("usage: tcprecv04 [ <host> ] <port#>");
    }

/*
如果等到accept返回之后再在已连接套接字上开启SO_OOBINLINE套接字选项，那时三路握手已经完成，带外数据也可能已经到达。
因此必须在监听套接字上开启这个选项，因为我们知道所有套接字选项会从监听套接字传承给已连接套接字
*/
    Setsockopt(listenfd, SOL_SOCKET, SO_OOBINLINE, &on, sizeof(on));

    connfd = Accept(listenfd, NULL, NULL);

    sleep(5);

    for(;;)
    {
        if(Sockatmark(connfd))
        {
            printf("at OOB mark\n");
        }

        if((n = Read(connfd, buff, sizeof(buff) - 1)) == 0)
        {
            printf("received EOF\n");

            return 0;
        }

        buff[n] = 0;

        printf("read %d bytes: %s\n", n, buff);
    }
}