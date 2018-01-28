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

void Writen(int fd, void *ptr, size_t nbytes)
{
	if (write(fd, ptr, nbytes) != nbytes)
		err_sys("writen error");
}

ssize_t Readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t	n;

	if ( (n = readline(fd, ptr, maxlen)) < 0)
		err_sys("readline error");
	return(n);
}

