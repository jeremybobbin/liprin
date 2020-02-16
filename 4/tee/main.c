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
	int opt, oflags, nread, nwritten, i, *fds, nfd;
	char buf[BUFSIZ];

	argv0 = *argv;
	oflags = O_WRONLY | O_CREAT | O_TRUNC;
	mode_t mode = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH; /* rw-r--r-- */
	
	while ((opt = getopt(argc, argv, "ad")) != -1)
	{
		switch (opt)
		{
			case 'a': /* append - remove truncate flag, add append flag */
				oflags = (~O_TRUNC & oflags) | O_APPEND;
				break;
		}
	}


	/* allocate descriptor array */
	nfd = 1 + argc - optind; /* one extra file descriptor for stdout */
	if ((fds = malloc(sizeof(int) * nfd)) == NULL)
		die(1, "could not allocate file descripters array");
	fds[0] = 1;

	/* open file descriptors */
	for (i = optind; i < argc; i++)
		if ((fds[(i+1)-optind] = open(argv[i], oflags, mode)) == -1)
			die(1, "could not open file %s", argv[i]);

	for (i = 0; i < nfd; i++)
		fprintf(stderr, "fd: %d\n", fds[i]);

	/* read from stdin & write to files */
	while ((nread = read(0, buf, BUFSIZ)) > 0)
		for (i = 0; i < nfd; i++)
			if ((nwritten = write(fds[i], buf, nread)) < nread)
				die(1, "read %d but only wrote %d on descriptor %d", nread, nwritten, fds[i]);

	/* close file descriptors */
	for (i = 0; i < nfd; i++)
		if (close(fds[i]) == -1)
			error("could not close descriptor %d", fds[i]);

}
