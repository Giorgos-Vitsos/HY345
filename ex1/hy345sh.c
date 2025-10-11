/**
 * @file hy345sh.c
 * @author Γιώργος Βίτσος (csd5369)
 * @brief A simple shell implementation in C
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int my_cd(char **args);
int my_help(char **args);
int my_exit(char **args);
int my_echo(char **args);
int my_pwd(char **args);
int my_env(char **args);

typedef struct{
    char *name;
    int (*func)(char **);
} buildin;

buildin builtins[] = {
    {"cd", my_cd},
    {"help", my_help},
    {"exit", NULL},
    {"echo", NULL},
    {"pwd", NULL},
    {"env", NULL},
    {NULL, NULL}
};

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
        getlogin_r(user, sizeof(user));//only on linux systems  
    #endif

    printf("<%s>@<csd5369>-hy345sh:<%s>$", user, dir);

}

void execute(char **args){
    char *current;
    int i=0;
    while(current=builtins[i].name){
        if(strcmp(args[0], current)==0){
            if(builtins[i].func!=NULL){
                builtins[i].func(args);
            }
            return;
        }
        i++;
    }


}

int my_cd(char **args){
    if(args[1]==NULL){
        printf("Expected argument after cd\n");  
    }else{
        if(chdir(args[1])){
            printf("No such file or directory\n");
        }
    }
    return 1;
}

int my_help(char **args){
    printf("The built-in commands are:\n");
    for(int i=0; builtins[i].name!=NULL; i++){
        printf("• %s\n", builtins[i].name);
    }

}



int main(){
    char *input;
    char **args;
    prompt();
    while((input=readline())){ 
        args=tokenize(input);
        execute(args);

        free(args); 
        free(input);
        prompt();
    }
}