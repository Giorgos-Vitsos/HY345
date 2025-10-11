/**
 * @file hy345sh.c
 * @author Γιώργος Βίτσος (csd5369)
 * @brief A simple shell implementation in C
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct{
    char *name;
    int (*func)(char **);
} buildin;

buildin builtins[] = {
    {"cd", NULL},
    {"help", NULL},
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


}

int main(){
    char *input;
    char **args;
    prompt();
    while((input=readline())){ 
        args=tokenize(input);
         for (int i = 0; args[i] != NULL; i++) {
            printf("Token[%d]: %s\n", i, args[i]);
        }

        free(args); 
        free(input);
        prompt();
    }
}