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
#include <fcntl.h>

char *get_var(char *name);
char **split_pipes(char *input);
void redirect(int old, int new);
int redirect_check(char **args);
void handle_redirect(char **args);

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

    for(int i=0; tokens[i]!=NULL; i++){//remove quotes

        int len=strlen(tokens[i]);//length

        if(tokens[i][0]=='"' && tokens[i][len-1]=='"'){

            tokens[i][len-1]='\0';
            tokens[i]++;
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
int execute(char **args){

    char *current;//current command
    int i=0;
    int status=0;

    if(check_var(args[0])){//check if it's a variable assignment
        return status;
    }

    while(current=builtins[i].name){//cheks all built-in commands

        if(strcmp(args[0], current)==0){

            if(builtins[i].func!=NULL){
                builtins[i].func(args);
            }

            return status;

        }

        i++;

    }

    //execute non built-in commands
    pid_t pid=fork();

    if(pid==0){//child 

        if(redirect_check(args)){
            handle_redirect(args);//handle redirections
        }

        if(execvp(args[0], args)==-1){//execute
            perror("Error");
            exit(EXIT_FAILURE);
        }
    }else if(pid>0){
        waitpid(pid, &status, 0);//parent waits for child
    }else{
        perror("Error forking");
    }
    return WEXITSTATUS(status);
}

/*This function checks if there are pipes in the input*/
int pipe_check(char *input){

    if(strchr(input, '|')!=NULL){
        return 1;
    }

    return 0;

}


/*This function executes piped commands*/
void execute_pipes(char *input){

    char **cmds=split_pipes(input);//holds all commands
    int count=0;//number of commands

    for(int i=0; cmds[i]!=NULL ; i++){
        count++;
    }

    int pipes[count][2];

    for(int i=0; i<count; i++){//create pipes

        pipe(pipes[i]);
    }

    pid_t pids[count];//holds all pids so that we can wait

    for(int i=0; i<count; i++){//for all commands

        pid_t pid;

        pid=fork();

        if(pid==0){//child

            char **args=tokenize(cmds[i]);//split command

            if(i!=0){//the first has stdin
                redirect(pipes[i-1][0], 0);
            }

            if(i!=count-1){//the last has stdout
                redirect(pipes[i][1], 1);
            }

            for(int j=0; j<count; j++){//close all child pipes

                close(pipes[j][0]);
                close(pipes[j][1]);

            }

            if(execvp(args[0], args)==-1){//execute

                perror("Error");
                exit(EXIT_FAILURE);

            }

        }else{//parent process

            pids[i]=pid;//store pid

        }
    }

    for(int i=0; i<count; i++){//close all parent pipes

        close(pipes[i][0]);
        close(pipes[i][1]);

    }

    for(int i=0; i<count; i++){//wait for all children

        waitpid(pids[i], NULL, 0);
    }

    free(cmds);
}

/*This function redirects input/output*/
void redirect(int old, int new){

    if(old!=new){
        dup2(old, new);
        close(old);
    }

}

/*This function split pipe commands*/
char **split_pipes(char  *input){

    char **tokens;
    int i=0;
    tokens=malloc(64 * sizeof(char*));

    if(!tokens){

        printf("Error\n");

    }

    char *token=strtok(input, "|");

    while(token!=NULL){

        tokens[i]=token;
        i++;
        token=strtok(NULL, "|");

    }

    tokens[i]=NULL;
    return tokens;

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

/*This function checks for redirections*/
int redirect_check(char **args){

    for(int i=0; args[i]!=NULL; i++){

        if(strcmp(args[i], ">")==0 || strcmp(args[i], ">>")==0 || strcmp(args[i], "<")==0){

            return 1;

        }
        
    }
    return 0;

}

/*This function handles redirections*/
void handle_redirect(char **args){

    int in=-1;//input fd
    int out=-1;//output fd

    for(int i=0; args[i]!=NULL;){//we check all arguments 

        if(strcmp(args[i], "<")==0){

            in=open(args[i+1], O_RDONLY);//open file

            if(in<0){
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }
            redirect(in,0);
            for(int j=i; args[j]!=NULL; j++){
                args[j]=args[j+2];
            }
            continue;

        }else if(strcmp(args[i], ">")==0){

            out=open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

            if(out<0){
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }
            redirect(out,1);
            for(int j=i; args[j]!=NULL; j++){
                args[j]=args[j+2];
            }
            continue;
        }else if(strcmp(args[i], ">>")==0){

            out=open(args[i+1], O_WRONLY | O_CREAT | O_APPEND, 0644);

            if(out<0){
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }
            redirect(out,1);
            for(int j=i; args[j]!=NULL; j++){
                args[j]=args[j+2];
            }
            continue;
        }else{
            i++;
        } 
    }
}

void handle_if(char *cmd){
    char **args;
    char **condition;
    int found_then=-1;
    int found_fi=-1;

    args=tokenize(cmd+3);
    for(int i=0; args[i]!=NULL; i++){
        if(strcmp(args[i], "then")==0){
            found_then=i;
        }
        if(strcmp(args[i], "fi")==0){
            found_fi=i;
        }
    }
    if((found_then==-1) || (found_fi==-1)){
        printf("Error: the syntax is wrong\n");
        return;
    }else{
        args[found_then]=NULL;
        args[found_fi]=NULL;
        condition=args;
        for(int i=0; condition[i]!=NULL; i++){
            printf("Condition Arg %d: %s\n", i, condition[i]);
        }
        for(int i=found_then+1; args[i]!=NULL; i++){
            printf("If Body Arg %d: %s\n", i-(found_then+1), args[i]);
        }
    }
}

void handle_for(char *cmd){

    char **args;
    args=tokenize(cmd);
    printf("Handling for statement: %s\n", cmd);
    printf("Tokenized args:\n");
    for(int i=0; args[i]!=NULL; i++){
        printf("Arg %d: %s\n", i, args[i]);
    }
}

char *flow_gather(char *input,char *key){

    char *block = strdup(input);
    char line[1024];

    while (1){
            printf("> ");
            if (!fgets(line, sizeof(line), stdin))
                break;

            block = realloc(block, strlen(block) + strlen(line) + 2);
            strcat(block, line);

            if ((strstr(line, key) != NULL) || (strstr(line, key) != NULL)){
                break;
            }
    }
    return block;
}

int main(){

    char *input;
    char **args;
    char **cmds;

    prompt();

    while((input=readline())){

        if((strncmp(input, "if", 2) == 0)){

            char *block =flow_gather(input,"fi");
            handle_if(block);
            free(block);
            prompt();
            continue;
        }
        if((strncmp(input, "for", 3) == 0)){

            char *block =flow_gather(input,"done");
            handle_for(block);
            free(block);
            prompt();
            continue;
        }

        cmds=split_commands(input);//split by ;
        
        for(int i=0; cmds[i]!=NULL; i++){//for all commands

            if(pipe_check(cmds[i])){//checks for pipes

                execute_pipes(cmds[i]);

            }else{//normal

                args=tokenize(cmds[i]);
                execute(args);
                free(args);

            }
        }
        
        free(cmds); 
        free(input);
        prompt();
    }
}
