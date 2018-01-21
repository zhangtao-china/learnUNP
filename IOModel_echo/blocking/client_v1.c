#include <string.h>
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
存在的问题：
1.当FIN到达套接字时客户阻塞在某个IO中，FIN没有被处理
2.客户阻塞在某个特定的IO中，这是不应该的(select和poll的目的就是解决这样的问题)。
3.当客户第一次向已经接收了FIN的套接字写时，服务器将回复一个RST，
  当客户已收到RST套接字执行写操作时，内核向该进程发送一个SIGPIPE信号，该信号的默认行为是终止进程。
详细描述：
当服务器进程终止后，服务器会向客户发送一个FIN，而客户TCP则响应一个ACK。
然而此时客户正阻塞在fgets上，等待从终端接收一行文本。当获取文本后，将调用write把数据发送给服务器。
当服务器TCP接收到来自客户的数据时，既然先前打开的那个调节子的进程已经终止，于是响应一个RST。
然而客户进程看不到这个RST，因为它在调用write后立即调用read，并且由于第二部接收的FIN，所调用的read立即返回0(表示EOF)。
客户此时并未预期收到EOF，于是以出错信息"server terminated prematurely"(服务器过早终止)退出。
*/
    while(Fgets(sendline, MAXLINE, fp) != NULL)
    {
        Writen(sockfd, sendline, strlen(sendline));

        if(Readline(sockfd, recvline, MAXLINE) == 0)
        {
            err_quit("str_cli: server terminated prematurely");
        }

        Fputs(recvline, stdout);
    }
}