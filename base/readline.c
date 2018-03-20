#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include "wrapunix.h"
#include "readline.h"
#include "commondef.h"
#include "wrapthread.h"
#include "error.h"
#include "wrappthread.h"

static pthread_key_t rl_key;
static pthread_once_t rl_once = PTHREAD_ONCE_INIT;

static void readline_destructor(void *ptr)
{
	free(ptr);
}

static void readline_once()
{
	Pthread_key_create(&rl_key, readline_destructor);
}

typedef struct
{
	int rl_cnt;
	char *rl_bufptr;
	char rl_buf[MAXLINE];
}Rline;

static ssize_t my_read(Rline *tsd, int fd, char *ptr)
{
	if(tsd->rl_cnt <= 0)
	{
again:
        if((tsd->rl_cnt = read(fd, tsd->rl_buf, MAXLINE)) < 0)
		{
			if(errno == EINTR)
			{
				goto again;
			}

			return -1;
		}
		else if(tsd->rl_cnt == 0)
		{
			return 0;
		}

		tsd->rl_bufptr = tsd->rl_buf;
	}

	tsd->rl_cnt--;
	*ptr = *tsd->rl_bufptr++;

	return 1;
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	readline_once();

	size_t n, rc;
	char c, *ptr;
	Rline *tsd;
	Pthread_once(&rl_once, readline_once);

	if((tsd = pthread_getspecific(rl_key)) == NULL)
	{
		tsd = Calloc(1, sizeof(Rline));
		Pthread_setspecific(rl_key, tsd);
	}

	ptr = vptr;
	for(n = 1; n < maxlen; n++)
	{
		if((rc = my_read(tsd, fd, &c)) == 1)
		{
			*ptr++ = c;
			if(c == '\n')
			{
				break;
			}
		}
		else if(rc == 0)
		{
			*ptr = 0;

			return n - 1;
		}
		else
		{
			return -1;
		}
	}

	*ptr = 0;

	return n;
}

#if 0
/*
不是线程安全的
*/
static ssize_t my_read(int fd, char *ptr)
{
	static int	read_cnt = 0;
	static char	*read_ptr;
	static char	read_buf[MAXLINE];

	if (read_cnt <= 0) 
	{
		if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0)
			err_sys("read error");
		else if (read_cnt == 0)
			return(0);
		read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++ & 255;
	return(1);
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	int		n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) 
	{
		if ( (rc = my_read(fd, &c)) == 1) 
		{
			*ptr++ = c;
			if (c == '\n')
				break;
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else
			return(-1);	/* error */
	}

	*ptr = 0;
	return(n);
}
#endif


#if 0
/*
系统调用过于频繁，效率极低
*/

ssize_t Readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t		n;

	if ( (n = readline(fd, ptr, maxlen)) == -1)
		err_sys("readline error");
	return(n);
}
#endif 
