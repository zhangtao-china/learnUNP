#include <string.h>
#include <unistd.h>
#include "../../base/wrapsocket.h"
#include "../../base/error.h"
#include "../../base/commondef.h"
#include "../../base/wrapio.h"

void str_cli(FILE *fp, int);

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        err_quit("usage: tcpcli <IPaddress>");
    }

    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr.s_addr);

    Connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);

    return 0;
}

void str_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];

/*
此版本为解决v1版本中的第三个问题，即向收到RST的套接字执行写操作内核会给进程发送SIGPIPE信号导致进程终止。
解决办法是对于一次发送调用两次write，第一次把文本行数据的第一个字节写入套接字，暂停一秒钟后，第二次把文本行
中剩余字节写入套接字。目的是让第一次write引发RST，再让第二个write产生SIGPIPE
*/
    while(Fgets(sendline, MAXLINE, fp) != NULL)
    {
        Writen(sockfd, sendline, 1);
        sleep(1);
        Writen(sockfd, sendline + 1, strlen(sendline) - 1);

        if(Readline(sockfd, recvline, MAXLINE) == 0)
        {
            err_quit("str_cli: server terminated prematurely");
        }

        Fputs(recvline, stdout);
    }
}