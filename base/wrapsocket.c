#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "wrapsocket.h"
#include "error.h"

int Socket(int family, int type, int protocol)
{
	int	n;

	if ( (n = socket(family, type, protocol)) < 0)
		err_sys("socket error");
	return(n);
}

void Bind(int sockfd, const sockaddr * addr, socklen_t addrlen)
{
    if (bind(sockfd, addr, addrlen) < 0)
    {
        err_sys("bind error"); 
    }
		
}

void Connect(int sockfd, const sockaddr *addr, socklen_t addrlen)
{
    if (connect(sockfd, addr, addrlen) < 0)
    {
        err_sys("connect error");
    }
}

void Listen(int listenfd, int backlog)
{
	char	*ptr;

	if ( (ptr = getenv("LISTENQ")) != NULL)
    {
        backlog = atoi(ptr);
    }
	
	if (listen(listenfd, backlog) < 0)
    {
        err_sys("listen error");
    }		
}

int	Accept(int listenfd, sockaddr * cliaddr, socklen_t * addrlen)
{
	int	n;

again:
	if ( (n = accept(listenfd, cliaddr, addrlen)) < 0) 
    {
#ifdef	EPROTO
		if (errno == EPROTO || errno == ECONNABORTED)
#else
		if (errno == ECONNABORTED)
#endif  
        {
            goto again;
        }
		else
        {
            err_sys("accept error");
        }
	}
	return(n);
}

void Inet_pton(int family, const char *strptr, void *addrptr)
{
	int		n;

	if ( (n = inet_pton(family, strptr, addrptr)) < 0)
    {
        err_sys("inet_pton error for %s", strptr);	/* errno set */
    }
		
	else if (n == 0)
    {
        err_quit("inet_pton error for %s", strptr);	/* errno not set */
    }
}

const char * Inet_ntop(int family, const void *addrptr, char *strptr, size_t len)
{
	const char	*ptr;

	if (strptr == NULL)		/* check for old code */
    {
        err_quit("NULL 3rd argument to inet_ntop");
    }
		
	if ( (ptr = inet_ntop(family, addrptr, strptr, len)) == NULL)
    {
        err_sys("inet_ntop error");		/* sets errno */
    }
		
	return ptr;
}

static int sockfd_to_family(int sockfd)
{
	struct sockaddr_storage ss;
	socklen_t len;

	len = sizeof(ss);
	if (getsockname(sockfd, (sockaddr *) &ss, &len) < 0)
    {
        return(-1);
    }
		
	return ss.ss_family;
}

int Sockfd_to_family(int sockfd)
{
	int	rc;

	if ( (rc = sockfd_to_family(sockfd)) < 0)
    {
        err_sys("sockfd_to_family error");
    }
		
	return rc;
}

int Getpeername(int connfd, sockaddr *peeraddr, socklen_t *addrlen)
{
    int res = getpeername(connfd, peeraddr, addrlen);
    if(res < 0)
    {
        err_sys("getpeername error");
    }
}

int Getsockname(int connfd, sockaddr *localaddr, socklen_t *addrlen)
{
    int res = getsockname(connfd, localaddr, addrlen);
	if (res < 0)
    {
        err_sys("getsockname error");
    }
}

void Shutdown(int fd, int how)
{
	if (shutdown(fd, how) < 0)
    {
        err_sys("shutdown error");
    }
}
