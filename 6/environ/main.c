#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "env.h"

char *argv0;

int main(int argc, char *argv[])
{
	jsetenv("SHELL", "/bin/ksh", 1);
	jsetenv("ZZZZZ", "zzzzzzzz", 0);
	jprintenv();
}
