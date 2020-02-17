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

int jdup(int fd)
{
	return fcntl(fd, F_DUPFD, 3);
}

int jdup3(int oldfd, int newfd, int flags)
{
	/* if newfd is open, close it */
	if (fcntl(newfd, F_GETFD) != -1)
		if (close(newfd) == -1)
			return -1;

	if (flags & FD_CLOEXEC)
		return fcntl(oldfd, F_DUPFD_CLOEXEC, newfd);
	else
		return fcntl(oldfd, F_DUPFD, newfd);
}

int jdup2(int oldfd, int newfd)
{
	return jdup3(oldfd, newfd, 0);
}

int dup_assert(int fd1, int fd2)
{
	return fcntl(fd1, F_GETFD) == fcntl(fd2, F_GETFD) &&
		lseek(fd1, 0, SEEK_CUR) == lseek(fd2, 0, SEEK_CUR);
}


void dup_assert_v(int fd1, int fd2)
{
	int a1, a2;

	if ((a1 = fcntl(fd1, F_GETFD)) == -1)
		die(1, "could not get fd1 flags");
	if ((a2 = fcntl(fd2, F_GETFD)) == -1)
		die(1, "could not get fd2 flags");

	fprintf(stderr, "Flags 1: %d\nFlags 2: %d\n\n", a1, a2);

	/* offset one to assert movement of the other */
	lseek(fd1, 100, SEEK_SET);

	if ((a1 = lseek(fd1, 0, SEEK_CUR)) == -1)
		die(1, "could not seek fd1");
	if ((a2 = lseek(fd2, 0, SEEK_CUR)) == -1)
		die(1, "could not seek fd2");

	lseek(fd1, -100, SEEK_SET);

	fprintf(stderr, "Offset 1: %d\nOffset 2: %d\n\n", a1, a2);

	if (dup_assert(fd1, fd2))
		fprintf(stderr, "File descriptors are equal!\n");
	else
		fprintf(stderr, "File descriptors are NOT equal!\n");
}

int main(int argc, char *argv[])
{
	int oflags, nread, nwritten, i, fd1, fd2, fd3;

	argv0 = *argv;
	oflags = O_WRONLY | O_CREAT;
	mode_t mode = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH; /* rw-r--r-- */

	fd1 = eopen("/tmp/dup", oflags, mode);
	lseek(fd1, 100, SEEK_SET);
	fd2 = jdup(fd1);

	dup_assert_v(fd1, fd2);
	jdup2(fd2, fd3 = 10);
	dup_assert_v(fd1, fd3);
	jdup3(fd3, fd2, 0);
	dup_assert_v(fd1, fd2);
	
	/* close file descriptors */
	eclose(fd1);
	eclose(fd2);
}
