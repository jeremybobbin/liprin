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

struct iovec {
	void *iov_base;
	size_t iov_len;
};

ssize_t iov_op(ssize_t(*fp)(int, void *, size_t), int fd, const struct iovec *iov, int iovcnt)
{
	int i, cnt, total;
	for (i = 0; i < iovcnt; i++)
		if ((cnt = fp(fd, iov[i].iov_base, iov[i].iov_len)) > 0)
			total += cnt;
		else break;
	return total;
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
{
	return iov_op(read, fd, iov, iovcnt);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
	return iov_op(write, fd, iov, iovcnt);
}





int main(int argc, char *argv[])
{
	int oflags, nread, nwritten, i, fd1, fd2, fd3;

	argv0 = *argv;
	oflags = O_WRONLY | O_CREAT;
	mode_t mode = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH; /* rw-r--r-- */
}
