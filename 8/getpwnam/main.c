#include <shadow.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

struct passwd *jgetpwnam(char *nam)
{
	struct passwd *pwd;
	while ((pwd = getpwent()) != NULL)
		if (strcmp(pwd->pw_name, nam) == 0)
			return pwd;

	return NULL;
}

int main(int argc, char *argv[])
{
	struct passwd *pwd;
	pwd = jgetpwnam("jer");
	printf("Name: %s\n", pwd->pw_name);
	printf("Password: %s\n", pwd->pw_passwd);
	printf("UID: %d\n", pwd->pw_uid);
	printf("GID: %d\n", pwd->pw_gid);
	printf("Comment: %s\n", pwd->pw_gecos);
	printf("Dir: %s\n", pwd->pw_dir);
	printf("Shell: %s\n", pwd->pw_shell);
}
