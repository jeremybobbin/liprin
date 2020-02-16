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

int main(int argc, char *argv[])
{
	int opt, sflags, dflags, nread, nwritten, i, *fds, nfd, src, dst;
	char buf[BUFSIZ];

	argv0 = *argv;
	sflags = O_RDONLY;
	dflags = O_WRONLY | O_CREAT | O_TRUNC;
	mode_t mode = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH; /* rw-r--r-- */
	
	if (argc != 3)
		die(1, "requires two arguments");

	/* open file descriptors */

	src = eopen(argv[1], sflags, 0);
	dst = eopen(argv[2], dflags, mode);

	/* read from stdin & write to files */
	while ((nread = read(src, buf, BUFSIZ)) > 0)
		if ((nwritten = write(dst, buf, nread)) < nread)
			die(1, "read %d but only wrote %d on descriptor %d", nread, nwritten, dst);

	/* close file descriptors */
	eclose(src);
	eclose(dst);
}
