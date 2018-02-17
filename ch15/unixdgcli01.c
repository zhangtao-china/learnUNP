#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <sys/un.h>
#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "../base/error.h"
#include "../base/commondef.h"

void dg_cli(FILE *fp, int sockfd, const sockaddr *pservaddr, socklen_t servlen);

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_un cliaddr, servaddr;

    sockfd = Socket(AF_LOCAL, SOCK_DGRAM, 0);

    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sun_family = AF_LOCAL;
    /*
    compile warning:
    he use of `tmpnam' is dangerous, better use `mkstemp'
    */
    strcpy(cliaddr.sun_path, tmpnam(NULL));

    /*
    与UDP不同，当使用UNIX域数据包协议时，必须显示bind一个路径名到我们的套接字，这样服务器才会有可能回射应答的路径名。
    */
    Bind(sockfd, (sockaddr *)&cliaddr, sizeof(cliaddr));

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXDG_PATH);

    dg_cli(stdin, sockfd, (sockaddr *)&servaddr, sizeof(servaddr));
}

void dg_cli(FILE *fp, int sockfd, const sockaddr *pservaddr, socklen_t servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    struct sockaddr_storage cliaddr;
    socklen_t len;
    char buf[SOCKADDR_STR_BUF_LEN];
    while(Fgets(sendline, MAXLINE, fp) != NULL)
    {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        len = sizeof(cliaddr);
        n = Recvfrom(sockfd, recvline, MAXLINE, 0, (sockaddr *)&cliaddr, &len);

        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
}