#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

int my_cd(char **args);
int my_help(char **args);
int my_exit(char **args);

/*Structure for built-in commands*/
typedef struct{
    char *name;
    int (*func)(char **);
} builtin;

extern builtin builtins[];

char *get_var(char *name);
char **split_pipes(char *input);
void redirect(int old, int new);
int redirect_check(char **args);
void handle_redirect(char **args);
int matches_key(char *input,char *key);