#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

char *argv0;

void verror(char *fmt, va_list ap)
{
	if (argv0)
		fprintf(stderr, "%s: ", argv0);
	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);
}

void die(int status, char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	verror(fmt, ap);
	va_end(ap);

	exit(status);
}

void error(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	verror(fmt, ap);
	va_end(ap);
}

int eopen(char *fname, int oflags, mode_t mode)
{
	int fd;
	if ((fd = open(fname, oflags, mode)) == -1)
		die(1, "could not open file %s", fname);

	return fd;
}

int eclose(int fd)
{
	if (close(fd) == -1)
		error("could not close descriptor %d", fd);
}

#define MAX(a, b) a > b ? a : b
#define MIN(a, b) a < b ? a : b

int main(int argc, char *argv[])
{
	int opt, oflags, nread, nwritten, i, fd, nbytes;
	char buf[BUFSIZ];
	for (i = 0; i < BUFSIZ; i++)
		buf[i] = 255;

	argv0 = *argv;
	oflags = O_WRONLY | O_CREAT;
	mode_t mode = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH; /* rw-r--r-- */
	
	if (argc < 3 || argc > 4)
		die(1, "provide two or three arguments");

	if (argc == 3)
		oflags |= O_APPEND;

	fd = eopen(argv[1], oflags, mode);

	if (argc == 4)
	{
		fprintf(stderr, "using lseek\n");
		lseek(fd, 0, SEEK_END);
	}

	if ((nbytes = atoi(argv[2])) == 0)
		die(1, "will not write zero bytes");
	else if (nbytes < 0)
		die(1, "will not write negetive bytes");

	/* read from stdin & write to files */
	while (nbytes > 0)
		if ((nwritten = write(fd, buf, MIN(BUFSIZ, nbytes))) <= 0)
			die(1, "write returned <= 0");
		else
			nbytes -= nwritten;
			
	/* close file descriptors */
	eclose(fd);
}
