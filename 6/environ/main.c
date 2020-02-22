#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


char *argv0;

int envlen = -1;
int envcap = -1;
extern char **environ;
char **jenviron;

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

char **initenv()
{
	int len;
	char **e, **j;

	if (envcap != -1 && envlen != -1 && jenviron != NULL)
		return jenviron;
	for (e = environ; *e; e++);
	envcap = envlen = (e - environ) - 1;
	if ((jenviron = malloc(sizeof(char *) * (envcap+1))) == NULL) 
		return NULL;
	for (e = environ, j = jenviron; *e; e++, j++)
	{
		len = strlen(*e);
		if ((*j = malloc(len * sizeof(char))) == NULL)
			return NULL;
		strcpy(*j, *e);
	}
	return jenviron;
}

int growenv()
{
	if (initenv() == NULL)
		return -1;
	if ((jenviron = realloc(jenviron, 2 * envcap * sizeof(char *))) == NULL)
		return -1;
	return 0;
}

int jputenv(char *s)
{
	if ((envcap - envlen) < 1)
		if (growenv() != 0)
			return -1;

	jenviron[envlen++] = s;
	jenviron[envlen] = NULL;
	return 0;
}

char *jgetenv(char *s)
{
	char **e;
	int nlen = strlen(s);
	if (initenv() == NULL)
		return NULL;
	for (e = jenviron; *e; e++)
		if (strncmp(s, *e, nlen) == 0 && *e && (*e)[nlen] == '=')
			return *e+nlen+1;
}


void jprintenv()
{
	char **e;
	for (e = jenviron; *e; e++)
		puts(*e);
}



int main(int argc, char *argv[])
{
	jprintenv();
}
