#ifndef WRAP_UNIX_H
#define WRAP_UNIX_H

#include <unistd.h>

void Close(int);

pid_t Fork(void);

typedef	void Sigfunc(int);

Sigfunc * Signal(int, Sigfunc *);

#endif