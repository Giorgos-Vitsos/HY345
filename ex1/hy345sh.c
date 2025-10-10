/**
 * @file hy345sh.c
 * @author Γιώργος Βίτσος (csd5369)
 * @brief A simple shell implementation in C
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

int main(){
    char *input;
    while(1){
        prompt();
        input = readline();
        printf("You entered: %s", input);//for testing purposes
        free(input);


    }
}