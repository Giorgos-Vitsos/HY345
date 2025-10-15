/**
 * @file hy345sh.c
 * @author Γιώργος Βίτσος (csd5369)
 * @brief A simple shell implementation in C
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "builtin.h"
#include <sys/wait.h>


builtin builtins[] = {
    {"cd", my_cd},
    {"help", my_help},
    {"exit", my_exit},
    {NULL, NULL}
};

typedef struct {
    char *name;
    char *value;
} variable;

variable variables[100];
int var_count = 0;

/*This function reads the input*/
char *readline(){

    char *buf=NULL;
    size_t bufsize;

    //error handler
    if(getline(&buf, &bufsize, stdin) == -1){

        if(feof(stdin)){

            printf("EOF");

        }else {

            printf("Error reading line");

        }

    }

    return buf;

}

char **tokenize(char *input){
    char **tokens;
    int i=0;
    tokens=malloc(64 * sizeof(char*));
    if(!tokens){
        printf("Error\n");
    }
    char *token=strtok(input, " \n\t\a\r");
    while(token!=NULL){
        tokens[i]=token;
        i++;
        token=strtok(NULL, " \n\t\a\r");
    }
    tokens[i]=NULL;
    return tokens;
}

/*This function prints the promt*/
void prompt(){

    char user[BUFSIZ];//holds the username
    char dir[BUFSIZ];//holds the workspace

    getcwd(dir, sizeof(dir));

    /*for testing in my windows system*/
    #ifdef _WIN32
        strcpy(user, "csd5369");
    #else
        strcpy(user,getenv("USER"));
    #endif

    printf("<%s>@<csd5369>-hy345sh:<%s>$", user, dir);

}

void set_var(char *name,char *value){
    if(var_count==100){
        printf("Variable limit reached\n");
        return;
    }else{
        variables[var_count].name=name;
        variables[var_count].value=value;
        var_count++;
    }
}

int check_var(char *args){
    if(strchr(args, '=')!=NULL){
        char *name=strtok(args, "=");
        char *value=strtok(NULL, "=");
        set_var(name, value);
        return 1;
    }
    return 0;
}



void execute(char **args){
    char *current;
    int i=0;
    if(check_var(args[0])){
        return;
    }
    while(current=builtins[i].name){
        if(strcmp(args[0], current)==0){
            if(builtins[i].func!=NULL){
                builtins[i].func(args);
            }
            return;
        }
        i++;
    }
    pid_t pid=fork();
    if(pid<=0){
        if(execvp(args[0], args)==-1){
            perror("Error");
        }
        exit(EXIT_FAILURE);
    }else{
        int status;
        wait(&status);
    }
}

char **split_commands(char  *input){
    char **tokens;
    int i=0;
    tokens=malloc(64 * sizeof(char*));
    if(!tokens){
        printf("Error\n");
    }
    char *token=strtok(input, ";");
    while(token!=NULL){
        tokens[i]=token;
        i++;
        token=strtok(NULL, ";");
    }
    tokens[i]=NULL;
    return tokens;
}




int main(){
    char *input;
    char **args;
    char **cmds;
    prompt();
    while((input=readline())){ 
        cmds=split_commands(input);
        for(int i=0; cmds[i]!=NULL; i++){
            printf("Command %d: %s\n", i+1, cmds[i]);
        }

        for(int i=0; cmds[i]!=NULL; i++){
            args=tokenize(cmds[i]);
            execute(args);
            free(args);
        }
        
        free(cmds); 
        free(input);
        prompt();
    }
}