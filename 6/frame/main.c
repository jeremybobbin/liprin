#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

char *argv0;

static int
square(int x)
{
	int result;
	result = x * x;
	return result;
}

static void
do_calc(int x)
{
	printf("The square of %d is %d\n", x, square(x));
	if (x < 1000) {
		int t;

		t = x * x * x;
		printf("The cube of %d is %d\n", x, t);
	}
}


#define LEN 10240000
int main(int argc, char *argv[])
{
	static int key = 9973;
	static char mbuf[LEN];
	mbuf[LEN-1] = '\0';

	char *p;
	p = malloc(1024);

	do_calc(key);

	exit(EXIT_SUCCESS);
}
