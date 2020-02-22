int jputenv(char *s);
char *jgetenv(char *s);
int jsetenv(const char *name, const char *value, int override);
void jprintenv();
int junsetenv();

