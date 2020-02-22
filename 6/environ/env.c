#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *argv0;

int envlen = -1;
int envcap = -1;
extern char **environ;
char **jenviron;

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
	return environ = jenviron;
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
	if (initenv() == NULL)
		return -1;
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


int jsetenv(const char *name, const char *value, int override)
{
	char **e, *d;

	/* (environment)? (name|value) length */
	int nlen, vlen, evlen;
	nlen = strlen(name);
	vlen = strlen(value);

	if (initenv() == NULL)
		return -1;

	/* locate variable */
	for (e = jenviron; *e; e++)
		if (strncmp(name, *e, nlen) == 0 && *e && (*e)[nlen] == '=')
			break;

	/* if (env var exists & if there's not enough space) realloc */
	if (*e) {
		if (override == 0)
			return -1;

		evlen = strlen(*e) - (nlen+1);

		/* if the new value len is bigger, realloc it */
		if (vlen > evlen)
			if ((*e = (char *) realloc(*e, (nlen+1+vlen) * sizeof(char))) == NULL)
				return -1;
		d = *e;
	} else {
		if ((d = (char *) malloc((nlen+1+vlen) * sizeof(char))) == NULL)
			return -1;
		sprintf(d, "%s=", name);
		jputenv(d);
	}

	strcpy(d + nlen+1, value);
	return 0;
}

int junsetenv(const char *name)
{
	char **e;
	int nlen = strlen(name);
	/* locate variable */
	for (e = jenviron; *e; e++)
		if (strncmp(name, *e, nlen) == 0 && *e && (*e)[nlen] == '=')
			break;

	free(*e);
	*e = jenviron[--envlen];
	jenviron[envlen] = NULL;
}

void jprintenv()
{
	initenv();
	char **e;
	for (e = jenviron; *e; e++)
		puts(*e);
}
