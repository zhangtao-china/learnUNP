#include <strings.h>
#include <sys/un.h>
#include <unistd.h>
#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "../base/error.h"
#include "../base/commondef.h"

void dg_echo(int sockfd, sockaddr *pcliaddr, socklen_t clilen);

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_un servaddr, cliaddr;

    sockfd = Socket(AF_LOCAL, SOCK_DGRAM, 0);

    unlink(UNIXDG_PATH);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXDG_PATH);
    
    Bind(sockfd, (sockaddr *)&servaddr, sizeof(servaddr));

    dg_echo(sockfd, (sockaddr *)&cliaddr, sizeof(cliaddr));
}

void dg_echo(int sockfd, sockaddr *pcliaddr, socklen_t clilen)
{
    int n;
    socklen_t len;
    char msg[MAXLINE];
    char strcli[SOCKADDR_STR_BUF_LEN];

    for(;;)
    {
        len = clilen;
        n = Recvfrom(sockfd, msg, MAXLINE, 0, pcliaddr, &len);

        if(sock_ntop(pcliaddr, len, strcli, sizeof(strcli)))
        {
            printf("[unixdgserv01] receive message from %s\n", strcli);
        }

        Sendto(sockfd, msg, n, 0, pcliaddr, len);
    }
}