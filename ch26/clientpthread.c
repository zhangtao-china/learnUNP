#include <strings.h>
#include <netinet/in.h>
#include <string.h>
#include "../base/wrapsocket.h"
#include "../base/error.h"
#include "../base/wrapio.h"
#include "../base/wrappthread.h"
#include "../base/wrapunix.h"

static int sockfd;
static FILE *fp;

void str_cli(FILE *fp_arg, int sockfd_arg);

int main(int argc, char **argv)
{
    int i, sockfd[5];
    struct sockaddr_in servaddr;

    if(argc != 2)
    {
        err_quit("usage: clientpthread <IPaddress>");
    }

    for(int i = 0; i < 5; i++)
    {
        sockfd[i] = Socket(AF_INET, SOCK_STREAM, 0);

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(SERV_PORT);
        Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

        Connect(sockfd[i], (sockaddr *)&servaddr, sizeof(servaddr));
    }

    str_cli(stdin, sockfd[0]);

    return 0;
}

void* copyto(void *arg)
{
    char sendline[MAXLINE];

    while(Fgets(sendline, MAXLINE, fp) != NULL)
    {
        Writen(sockfd, sendline, strlen(sendline));
    }

    Shutdown(sockfd, SHUT_WR);

    return NULL;
}

void str_cli(FILE *fp_arg, int sockfd_arg)
{
    char recvline[MAXLINE];
    pthread_t tid;

    sockfd = sockfd_arg;
    fp = fp_arg;

    Pthread_create(&tid, NULL, copyto, NULL);

    while(Readline(sockfd, recvline, MAXLINE) > 0)
    {
        Fputs(recvline, stdout);
    }
}