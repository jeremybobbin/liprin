#include <fcntl.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

static jmp_buf env;

void setjmp_return(int y)
{
	int x = 2;
	if (setjmp(env))
		printf("%d + %d is %d", x, y, x + y);
}

int main(int argc, char *argv[])
{
	setjmp_return(3);
	longjmp(env, 1);
}
