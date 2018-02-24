#include <stdlib.h>
#include "web.h"
#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "../base/wrapunix.h"

#define min(a, b) ((a) < (b) ? (a) : (b))

void home_page(const char *host, const char *fname);

int main(int argc, char **argv)
{
    int i, fd, n, maxnconn, flags, error;
    char buf[MAXLINE];
    fd_set rs, ws;

    if(argc < 5)
    {
        err_quit("usage: web <#conns> <hostname> <homepage> <file> ...");
    }

    maxnconn = atoi(argv[1]);
    nfiles = min(argc - 4, MAXFILES);

    for(i = 0; i < nfiles; i++)
    {
        file[i].f_name = argv[i + 4];
        file[i].f_host = argv[2];
        file[i].f_fd = 0;
    }

    printf("nfiles = %d\n", nfiles);

    home_page(argv[2], argv[3]);
    FD_ZERO(&rset);
    FD_ZERO(&wset);
    maxfd = -1;
    nlefttoread = nlefttoconn = nfiles;
    nconn = 0;

    while(nlefttoread > 0)
    {
        while(nconn < maxnconn && nlefttoconn > 0)
        {
            for(i = 0; i < nfiles; i++)
            {
                if(file[i].f_flags == 0)
                {
                    break;
                }
            }

            if(i == nfiles)
            {
                err_quit("nlefttoconn = %d bug nothing found", nlefttoconn);
            }

            start_connect(&file[i]);
            nconn++;
            nlefttoconn--;
        }
    

        rs = rset;
        ws = wset;
        n = Select(maxfd + 1, &rs, &ws, NULL, NULL);

        for(i = 0; i < nfiles; i++)
        {
            flags = file[i].f_flags;
            if(flags == 0 || flags & F_DONE)
            {
                continue;
            }

            fd = file[i].f_fd;
            if(flags & F_CONNECTING && (FD_ISSET(fd, &rs) || FD_ISSET(fd, &ws)))
            {
                n = sizeof(error);
                if(getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &n) < 0 || error != 0)
                {
                    err_ret("nonblocking connect failed or %s", file[i].f_name);
                }
                printf("connection established for %s\n", file[i].f_name);
                FD_CLR(fd, &wset);
                write_get_cmd(&file[i]);
            }
            else if(flags & F_READING && FD_ISSET(fd, &rs))
            {
                if(n == Read(fd, buf, sizeof(buf)) == 0)
                {
                    printf("end-of-file on %s\n", file[i].f_name);
                    Close(fd);
                    file[i].f_flags = F_DONE;
                    FD_CLR(fd, &rset);
                    nconn--;
                    nlefttoread--;
                }
                else
                {
                    printf("read %d bytes from %s\n", n, file[i].f_name);
                }
            }
        }
    }

    exit(0);
}

void home_page(const char *host, const char *fname)
{
    int fd, n;
    char line[MAXLINE];

    fd = tcp_connect(host, SERV);
    n = snprintf(line, sizeof(line), GET_CMD, fname);
    Writen(fd, line, n);

    for(;;)
    {
        if( (n = Read(fd, line, MAXLINE)) == 0)
        {
            break;
        }
    }

    printf("read %d bytes of home page\n", n);
    Close(fd);
}

void start_connect(struct file *fptr)
{
    int fd, flags, n;
    struct addrinfo *ai;

    ai = host_serv(fptr->f_host, SERV, 0, SOCK_STREAM);

    fd = Socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    fptr->f_fd = fd;
    printf("start_connect for %s, fd %d\n", fptr->f_name, fd);

    flags = Fcntl(fd, F_GETFL, 0);
    Fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    if((n = connect(fd, ai->ai_addr, ai->ai_addrlen)) < 0)
    {
        if(errno != EINPROGRESS)
        {
            err_sys("nonblocking connect error");
        }

        fptr->f_flags = F_CONNECTING;
        FD_SET(fd, &rset);
        FD_SET(fd, &wset);

        if(fd > maxfd)
        {
            maxfd = fd;
        }
    }
    else if(n >= 0)
    {
        write_get_cmd(fptr);
    }
}

void write_get_cmd(struct file *fptr)
{
    int n;
    char line[MAXLINE];
    n = snprintf(line, sizeof(line), GET_CMD, fptr->f_name);
    Writen(fptr->f_fd, line, n);
    printf("wrote %d bytes for %s\n", n, fptr->f_name);
    printf("wrote contents: %s", line);

    fptr->f_flags = F_READING;

    FD_SET(fptr->f_fd, &rset);
    if(fptr->f_fd > maxfd)
    {
        maxfd = fptr->f_fd;
    }
}

