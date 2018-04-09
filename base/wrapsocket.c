#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/un.h>
#include "wrapsocket.h"
#include "error.h"
#include "wrapunix.h"
#include "config.h"

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
	int	n;

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

char *sock_ntop(const sockaddr *sa, socklen_t salen, char *strptr, size_t len)
{   
    if(NULL == strptr)
    {
        return NULL;
    }

    char portstr[8];
    char str[128];		/* Unix domain is largest */

    char *pstr = str;

	switch (sa->sa_family) 
    {
	case AF_INET: 
    {
		struct sockaddr_in *sin = (struct sockaddr_in *) sa;

		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
        {  return NULL;  }
			
		if (ntohs(sin->sin_port) != 0) 
        {
			snprintf(portstr, sizeof(portstr), ":%hu", ntohs(sin->sin_port));
			strcat(str, portstr);
		}

        break;
	}
/* end sock_ntop */

#ifdef	IPV6
	case AF_INET6: 
    {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		str[0] = '[';
		if (inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1) == NULL)
        {  
            return NULL;  
        }

		if (ntohs(sin6->sin6_port) != 0)
        {
			snprintf(portstr, sizeof(portstr), "]:%hu", ntohs(sin6->sin6_port));
			strcat(str, portstr);
		}

		pstr = str + 1;

        break;
	}
#endif

#ifdef	AF_UNIX
	case AF_UNIX: 
    {
		struct sockaddr_un	*unp = (struct sockaddr_un *) sa;

			/* OK to have no pathname bound to the socket: happens on
			   every connect() unless client calls bind() first. */
		if (unp->sun_path[0] == 0)
		{
            strcpy(str, "(no pathname bound)");
        }
		else
        {
            snprintf(str, sizeof(str), "%s", unp->sun_path);
        }

        break;
	}
#endif

#ifdef	HAVE_SOCKADDR_DL_STRUCT
	case AF_LINK: 
    {
		struct sockaddr_dl	*sdl = (struct sockaddr_dl *) sa;

		if (sdl->sdl_nlen > 0)
        {
            snprintf(str, sizeof(str), "%*s (index %d)",
					 sdl->sdl_nlen, &sdl->sdl_data[0], sdl->sdl_index);
        }
		else
        {
            snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
        }

        break;
	}
#endif
	default:
    {
        snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d",
				 sa->sa_family, salen);
        return NULL;
    }
		break;
	}

    size_t reslen = strlen(pstr);
    if(len < reslen)
    {
        err_msg("[sock_ntop] buff length is not enough, buff length at least %u.", reslen);
        return NULL;
    }
    else
    {
        strcpy(strptr, pstr);
        return strptr;
    }
}

void Getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
    if(getsockopt(sockfd, level, optname, optval, optlen) < 0)
    {
        err_sys("getsockopt error");
    }
}

void Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    if(setsockopt(sockfd, level, optname, optval, optlen) < 0)
    {
        err_sys("setsockopt error");
    }
}

int tcp_connect(const char *host, const char * serv)
{
    int sockfd, n;
    struct addrinfo hints, *res, *ressave;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    {
        err_quit("tcp_connect error for %s, %s: %s", host, serv, gai_strerror(n));
    }

    ressave = res;

    do
    {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if(sockfd < 0)
        {
            continue;
        }

        if(connect(sockfd,res->ai_addr, res->ai_addrlen) == 0)
        {
            break;
        }

        Close(sockfd);
    }while((res = res->ai_next) != NULL);

    if(res == NULL)
    {
        err_sys("tcp_connect error for %s, %s", host, serv);
    }

    freeaddrinfo(ressave);

    return sockfd;
}

struct addrinfo * host_serv(const char *host, const char *serv, int family, int socktype)
{
    int n;
    struct addrinfo hints, *res;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = family;
    hints.ai_socktype = socktype;

    if((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    {
        return NULL;
    }

    return res;
}

int tcp_listen(const char *hostname, const char *service, socklen_t *addrlenp)
{
    int listenfd, n;
    const int on = 1;
    struct addrinfo hints, *res, *ressave;

    bzero(&hints, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if((n = getaddrinfo(hostname, service, &hints, &res)) != 0)
    {
        err_quit("tcp_listen error for %s, %s: %s", hostname, service, gai_strerror(n));
    }

    ressave = res;

    do
    {
        listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

        if(listenfd < 0)
        {
            continue;
        }

        Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if(bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
        {
            break;
        }

        Close(listenfd);
    }while((res = res->ai_next) != NULL);

    if(res == NULL)
    {
        err_sys("tcp_listen error for %s, %s", hostname, service);
    }

    Listen(listenfd, LISTENQ);

    if(addrlenp)
    {
        *addrlenp = res->ai_addrlen;
    }

    freeaddrinfo(ressave);

    return listenfd;
}


char * sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
    static char str[128];		/* Unix domain is largest */

	switch (sa->sa_family) 
    {
	case AF_INET: 
    {
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return NULL;
		return str;
	}

#ifdef	IPV6
	case AF_INET6: 
    {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL)
			return NULL;
		return str;
	}
#endif

#ifdef	AF_UNIX
	case AF_UNIX: 
    {
		struct sockaddr_un	*unp = (struct sockaddr_un *) sa;

			/* OK to have no pathname bound to the socket: happens on
			   every connect() unless client calls bind() first. */
		if (unp->sun_path[0] == 0)
			strcpy(str, "(no pathname bound)");
		else
			snprintf(str, sizeof(str), "%s", unp->sun_path);
		return str;
	}
#endif

#ifdef	HAVE_SOCKADDR_DL_STRUCT
	case AF_LINK:
    {
		struct sockaddr_dl	*sdl = (struct sockaddr_dl *) sa;

		if (sdl->sdl_nlen > 0)
			snprintf(str, sizeof(str), "%*s",
					 sdl->sdl_nlen, &sdl->sdl_data[0]);
		else
			snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
		return(str);
	}
#endif
	default:
		snprintf(str, sizeof(str), "sock_ntop_host: unknown AF_xxx: %d, len %d",
				 sa->sa_family, salen);
		return(str);
	}
    return (NULL);
}

int	Sockatmark(int fd)
{
    int n;

	if ( (n = sockatmark(fd)) < 0)
    {
        err_sys("sockatmark error");
    }
		
	return n;
}