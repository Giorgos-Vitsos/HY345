int my_cd(char **args);
int my_help(char **args);
int my_exit(char **args);

typedef struct{
    char *name;
    int (*func)(char **);
} builtin;

extern builtin builtins[];