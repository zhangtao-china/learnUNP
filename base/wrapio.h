#ifndef WRAP_IO_H
#define WRAP_IO_H

#include <stdio.h>

ssize_t Read(int, void *, size_t);

void Write(int, void *, size_t);

char* Fgets(char *ptr, int n, FILE *stream);

void Fputs(const char *ptr, FILE *stream);

void Writen(int fd, void *ptr, size_t nbytes);

ssize_t	Readline(int, void *, size_t);

#endif