#include <string.h>
#include <sys/un.h>
#include "../base/wrapsocket.h"
#include "../base/error.h"
#include "../base/commondef.h"
#include "../base/wrapio.h"

void str_cli(FILE *fp, int);

int main(int argc, char **argv)
{
    int sockfd = Socket(AF_LOCAL, SOCK_STREAM, 0);

    char localstr[SOCKADDR_STR_BUF_LEN];
    struct sockaddr_un servaddr, localaddr;
    socklen_t addrlen = sizeof(localaddr);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXSTR_PATH);

    Connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));
    
    if(Getsockname(sockfd, (sockaddr *)&localaddr, &addrlen) == 0)
    {
        if(sock_ntop((sockaddr *)&localaddr, sizeof(localaddr), localstr, sizeof(localstr)) != NULL)
        {
            printf("[unixstrcli01] local socket %s, connect to server: %s successfully.\n", localstr, UNIXSTR_PATH);
        }
    }

    str_cli(stdin, sockfd);

    return 0;
}

void str_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];

    while(Fgets(sendline, MAXLINE, fp) != NULL)
    {
        Writen(sockfd, sendline, strlen(sendline));

        if(Readline(sockfd, recvline, MAXLINE) == 0)
        {
            err_quit("[unixstrcli01] str_cli: server terminated prematurely");
        }

        Fputs(recvline, stdout);
    }
}