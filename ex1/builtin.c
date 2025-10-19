#include "builtin.h"

/*This function implements cd*/
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

/*This function implements help*/
int my_help(char **args){
    printf("The built-in commands are:\n");
    for(int i=0; builtins[i].name!=NULL; i++){
        printf("• %s\n", builtins[i].name);
    }

}

/*This function implements exit*/
int my_exit(char **args){
    printf("Exiting hy345sh...\n");
    exit(0);
}

