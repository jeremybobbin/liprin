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

int main(int argc, char *argv[])
{
	int opt, oflags, nread, nwritten, i, fd, nfd;
	char buf[BUFSIZ];

	argv0 = *argv;
	oflags = O_WRONLY | O_CREAT | O_APPEND ;
	mode_t mode = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH; /* rw-r--r-- */

	if (argc != 2)
		die(1, "provide one argument");

	if ((fd = open(argv[1], oflags, mode)) == -1)
		die(1, "could not open %s", argv[1]);

	if (lseek(fd, 0, SEEK_SET) == -1) 
		die(1, "could not seek");


	/* read from stdin & write to file */
	while ((nread = read(0, buf, BUFSIZ)) > 0)
		if ((nwritten = write(fd, buf, nread)) < nread)
			die(1, "read %d but only wrote %d on descriptor %d", nread, nwritten, fd);

	if (close(fd) == -1)
		error("could not close descriptor %d", fd);

}
