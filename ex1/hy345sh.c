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

char *get_var(char *name);

/*Array with named functions to loop easily*/
builtin builtins[] = {
    {"cd", my_cd},
    {"help", my_help},
    {"exit", my_exit},
    {NULL, NULL}
};

//Structure to hold variables
typedef struct {
    char *name;
    char *value;
} variable;

variable variables[100];//number of possible variables
int var_count = 0;//current number of variables

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

/*This function tokenizes the input*/
char **tokenize(char *input){

    char **tokens;//array of tokens
    int i=0;
    tokens=malloc(64 * sizeof(char*));

    if(!tokens){
        printf("Error\n");
    }

    char *token=strtok(input, " \n\t\a\r");//splits the input

    while(token!=NULL){//while there are more arguments
        tokens[i]=token;//store the token
        i++;
        token=strtok(NULL, " \n\t\a\r");//get the next token
    }

    tokens[i]=NULL;//null-terminate

    for(int i=0; tokens[i]!=NULL; i++){//search all tokens

        if(strchr(tokens[i], '$')!=NULL){//if one is a variable

            char *var_value=get_var(tokens[i]+1);

            if(var_value!=NULL){

                tokens[i]=var_value;//we replace it with its value

            }else{//if the variable does not exist

                tokens[i]=""; 
            }
        }
    }

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

/*Functions to set variables*/                                                          
void set_var(char *name,char *value){

    if(var_count==100){

        printf("Variable limit reached\n");
        return;

    }else{

        variables[var_count].name=strdup(name);
        variables[var_count].value=strdup(value);
        var_count++;

    }
}

/*Function to get variable value*/
char* get_var(char *name){

    char* value;

    for(int i=0; i<var_count; i++){//searches for the variable

        if(strcmp(variables[i].name, name)==0){

            value= variables[i].value;
            return value;

        }
    }

    printf("Variable not found\n");
    return NULL;

}

/*Function to check if the input is a variable assignment*/
int check_var(char *args){

    if(strchr(args, '=')!=NULL){

        char *name=strtok(args, "=");
        char *value=strtok(NULL, "=");
        set_var(name, value);
        return 1;

    }

    return 0;

}

/*This function executes the commands*/
void execute(char **args){

    char *current;//current command
    int i=0;

    if(check_var(args[0])){//check if it's a variable assignment
        return;
    }

    while(current=builtins[i].name){//cheks all built-in commands

        if(strcmp(args[0], current)==0){

            if(builtins[i].func!=NULL){

                builtins[i].func(args);

            }

            return;

        }

        i++;

    }

    //execute non built-in commands
    pid_t pid=fork();

    if(pid<=0){//child 

        if(execvp(args[0], args)==-1){
            perror("Error");
        }

        exit(EXIT_FAILURE);

    }else{//parent

        int status;
        wait(&status);
    }
}

/*This function splits commands by ; (very similar to tokenize)*/
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

            args=tokenize(cmds[i]);
            execute(args);
            free(args);

        }
        
        free(cmds); 
        free(input);
        prompt();
    }
}