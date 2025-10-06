#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


char *readline(){
    char *buf=NULL;
    size_t bufsize;
    if(getline(&buf, &bufsize, stdin) == -1){
        if(feof(stdin)){
            printf("EOF");
        } else {
            printf("Error reading line");
        }
    }
    return buf;
}

void promt(){
    char user[BUFSIZ];
    char dir[BUFSIZ];

    getcwd(dir, sizeof(dir));
    /*for testing in my windows system*/
    #ifdef _WIN32
        strcpy(user, "csd5369");
    #else
        getlogin_r(user, sizeof(user));   
    #endif
    printf("<%s>@<csd5369>-hy345sh:<%s>$", user, dir);

}


int main(){
    char *input;
    while(1){
        promt();
        input = readline();
        printf("You entered: %s", input);
        free(input);


    }
}