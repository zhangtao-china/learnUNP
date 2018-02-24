#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "../../base/wrapsocket.h"
#include "../../base/error.h"
#include "../../base/commondef.h"
#include "../../base/wrapio.h"
#include "../../base/wrapunix.h"

#define	max(a,b)  ((a) > (b) ? (a) : (b))

void str_cli(FILE *fp, int);
char* gf_time(void);


int main(int argc, char **argv)
{
    if(argc != 2)
    {
        err_quit("usage: tcpcli <IPaddress>");
    }

    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    char localstr[SOCKADDR_STR_BUF_LEN];
    struct sockaddr_in servaddr, localaddr;
    socklen_t addrlen = sizeof(localaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr.s_addr);

    Connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));
    
    if(Getsockname(sockfd, (sockaddr *)&localaddr, &addrlen) == 0)
    {
        if(sock_ntop((sockaddr *)&localaddr, addrlen, localstr, sizeof(localstr)) != NULL)
        {
            printf("[client_v4] local socket %s, connect to server: %s:%hu successfully.\n", 
                    localstr, argv[1], SERV_PORT);
        }
    }

    str_cli(stdin, sockfd);

    return 0;
}

void str_cli(FILE *fp, int sockfd)
{
    int maxfdp1, val, stdineof;
    ssize_t n, nwriten;
    fd_set rset, wset;
    char to[MAXLINE], fr[MAXLINE];
    char *toiptr, *tooptr, *friptr, *froptr;

    val = Fcntl(sockfd, F_GETFL, 0);
    Fcntl(sockfd, F_SETFL, val | O_NONBLOCK);

    val = Fcntl(STDIN_FILENO, F_GETFL, 0);
    Fcntl(STDIN_FILENO, F_SETFL, val | O_NONBLOCK);

    val = Fcntl(STDOUT_FILENO, F_GETFL, 0);
    Fcntl(STDOUT_FILENO, F_SETFL, val | O_NONBLOCK);

    toiptr = tooptr = to;
    friptr = froptr = fr;
    stdineof = 0;

    maxfdp1 = max(max(STDIN_FILENO, STDOUT_FILENO), sockfd) + 1;

    for(;;)
    {
        FD_ZERO(&rset);
        FD_ZERO(&wset);

        if(stdineof == 0 && toiptr < &to[MAXLINE])
        {
            FD_SET(STDIN_FILENO, &rset);
        }
        if(friptr < &fr[MAXLINE])
        {
            FD_SET(sockfd, &rset);
        }

        if(tooptr != toiptr)
        {
            FD_SET(sockfd, &wset);
        }
        if(froptr != friptr)
        {
            FD_SET(STDOUT_FILENO, &wset);
        }

        Select(maxfdp1, &rset, &wset, NULL, NULL);

        if(FD_ISSET(STDIN_FILENO, &rset))
        {
            if((n = read(STDIN_FILENO, toiptr, &to[MAXLINE] - toiptr)) < 0)
            {
                if(errno != EWOULDBLOCK)
                {
                    err_sys("read error on stdin");
                }
            }
            else if(n == 0)
            {
                fprintf(stderr, "%s: EOF on stdin\n", gf_time());
                stdineof = 1;
                if(tooptr == toiptr)
                {
                    Shutdown(sockfd, SHUT_WR);
                }
            }
            else
            {
                // fprintf(stderr, "%s: read %ld bytes from stdin\n", gf_time(), n);
                toiptr += n;
                FD_SET(sockfd, &wset);
            }
        }

        if(FD_ISSET(sockfd, &rset))
        {
            if((n = read(sockfd, friptr, &fr[MAXLINE] - friptr)) < 0)
            {
                if(errno != EWOULDBLOCK)
                {
                    err_sys("read error on socket");
                }
            }
            else if(n == 0)
            {
                fprintf(stderr, "%s: EOF on socket\n", gf_time());
                if(stdineof)
                {
                    return;
                }
                else
                {
                    err_quit("str_cli: server terminated preematurely");
                }
            }
            else 
            {
                // fprintf(stderr, "%s: read %ld bytes from socket\n", gf_time(), n);

                friptr += n;
                FD_SET(STDOUT_FILENO, &wset);
            }
        }

        if(FD_ISSET(STDOUT_FILENO, &wset) && ((n = friptr - froptr) > 0))
        {
            if((nwriten = write(STDOUT_FILENO, froptr, n)) < 0)
            {
                if(errno != EWOULDBLOCK)
                {
                    err_sys("write error to stdout");
                }
            }
            else 
            {
                // fprintf(stderr, "%s: wrote %ld bytes to stdout\n", gf_time(), nwriten);
                froptr += nwriten;
                if(froptr == friptr)
                {
                    froptr = friptr = fr;
                }
            }
        }

        if(FD_ISSET(sockfd, &wset) && ((n = toiptr - tooptr) > 0))
        {
            if((nwriten = write(sockfd, tooptr, n)) < 0)
            {
                if(errno != EWOULDBLOCK)
                {
                    err_sys("write error to socket");
                }
            }
            else 
            {
                // fprintf(stderr, "%s: wrote %ld bytes to socket\n", gf_time(), nwriten);
                tooptr += nwriten;
                if(tooptr == toiptr)
                {
                    toiptr = tooptr = to;
                    if(stdineof)
                    {
                        Shutdown(sockfd, SHUT_WR);
                    }
                }
            }
        }
    }
}

char* gf_time(void)
{
    struct timeval tv;
    static char str[30];
    char *ptr;

    if(gettimeofday(&tv, NULL) < 0)
    {
        err_sys("gettimeofday error");
    }

    ptr = ctime(&tv.tv_sec);
    strcpy(str, &ptr[11]);
    
    snprintf(str + 8, sizeof(str) - 8, ".%06ld", tv.tv_usec);

    return str;
}