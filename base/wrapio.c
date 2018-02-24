#include <unistd.h>
#include "wrapio.h"
#include "error.h"
#include "readline.h"

ssize_t Read(int fd, void *ptr, size_t nbytes)
{
	ssize_t n;

	if ( (n = read(fd, ptr, nbytes)) == -1)
	{
		err_sys("read error");
	}
		
	return n;
}

void Write(int fd, void *ptr, size_t nbytes)
{
	if (write(fd, ptr, nbytes) != nbytes)
	{
		err_sys("write error");
	}
}

char* Fgets(char *ptr, int n, FILE *stream)
{
	char *rptr;

	if ( (rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
    {
        err_sys("fgets error");
    }

	return (rptr);
}

void Fputs(const char *ptr, FILE *stream)
{
	if (fputs(ptr, stream) == EOF)
    {
        err_sys("fputs error");
    }
}

static ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0)
    {
		if ((nwritten = write(fd, ptr, nleft)) <= 0) 
		{
			if (nwritten < 0 && errno == EINTR)
			{
				nwritten = 0;		/* and call write() again */
			}
			else
			{
				return(-1);			/* error */
			}
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}

	return n;
}

void Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
	{
		err_sys("writen error");
	}
}

ssize_t Readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t	n;

	if ( (n = readline(fd, ptr, maxlen)) < 0)
	{
		err_sys("readline error");
	}
		
	return n;
}

ssize_t Recvfrom(int sockfd, void *buff, size_t nbytes, int flags, sockaddr *addr, socklen_t *addrlen)
{
	int n;
	if( (n = recvfrom(sockfd, buff, nbytes, flags, addr, addrlen)) < 0)
	{
		err_sys("recvfrom error");
	}
}

void Sendto(int sockfd, void *buff, size_t nbytes, int flags, const sockaddr *addr, socklen_t addrlen)
{
	int n;
	if( (n = sendto(sockfd, buff, nbytes, flags, addr, addrlen)) != nbytes)
	{
		err_sys("sendto error");
	}
}