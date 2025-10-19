/**
 * @file hy345sh.c
 * @author Γιώργος Βίτσος (csd5369)
 * @brief A simple shell implementation in C
 */

#include "builtin.h"

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
    if(getline(&buf, &bufsize, stdin)== -1){

        if(feof(stdin)){

            printf("EOF");

        }else {

            printf("Error reading line");

        }
    }

    return buf;

}

/*This function tokenizes the input*/
char **tokenize(char *input) {

    char **tokens=malloc(64 * sizeof(char*));//space for the tokens
    int i=0;

    if(!tokens){//error handler

        printf("Error\n");
        return NULL; 

    }

    int len=strlen(input);

    for (int j=0; j<len;){

        if(input[j] == ' ' || input[j] == '\t'){//we skip whitespace

            j++; 
            continue; 

        }


        if(input[j] == '<' || input[j] == '>'){//if there is a redirection

            if(input[j] == '>' && input[j+1] == '>'){//we check for >>

                tokens[i++]=strdup(">>"); 
                j += 2;//we skip 2

            }else{

                char op[2]={input[j], '\0'};//we get the char
                tokens[i++]=strdup(op); 
                j++;//we skip 1

            }

            continue;
        }

        int start = j;//beggining of word
        int toggle = 0;//for quotes

        while(j < len && (toggle || (input[j] != ' ' && input[j] != '\t' && input[j] != '<' && input[j] != '>'))){//we skip

            if (input[j] == '"'){
                toggle = !toggle;
            }
            j++;

        }

        int word_len = j - start;
        char *word = malloc(word_len + 1);
        strncpy(word, input + start, word_len);
        word[word_len] = '\0';

        char *src = word;//we remove quotes
        char *dst = word;

        while(*src){

            if(*src != '"'){

                *dst++=*src;

            }

            src++;
        }

        *dst = '\0';

        char buffer[1024] = {0};//buffer for variables
        char *p = word;
        char *b = buffer;

        while(*p){//check each char

            if(*p=='$'){

                p++;
                char varname[128]={0};
                int k = 0;

                while(*p && (isalnum(*p) || *p=='_')){//all valid chars are copied

                    varname[k++]= *p++;

                }

                varname[k] = '\0';
                char *val = get_var(varname);//we get the value

                if(val){ 

                    strcpy(b, val);//copy into buffer 
                    b+=strlen(val);//move index

                }

            }else{

                *b++=*p++;//copy 

            }
        }

        *b= '\0';
        free(word);
        tokens[i++] = strdup(buffer);

    }

    tokens[i] = NULL;
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

    for(int i = 0; i < var_count; i++){//check if already exists

        if(matches_key(variables[i].name,name)){

            free(variables[i].value);
            variables[i].value = strdup(value);
            return;
        }
    }

    if(var_count==100){

        printf("Variable limit reached\n");
        return;

    }else{//sets variable
        
        variables[var_count].name=strdup(name);
        variables[var_count].value=strdup(value);
        var_count++;

    }
}

/*Function to get variable value*/
char* get_var(char *name){

    char* value;

    for(int i=0; i<var_count; i++){//searches for the variable

        if(matches_key(variables[i].name,name)){

            value= variables[i].value;
            return value;

        }
    }

    printf("Variable not found\n");
    return NULL;

}

/*Checks if we have a variable and sets it*/
int check_var(char *args){

    char *eq=strchr(args, '=');//finds =

    if(!eq){//if not variable
        return 0;
    }

    *eq = '\0';//we delete =
    char *name=args;
    char *value=eq+1;//value starts after =

    char *src = value;//we delete the quotes
    char *dst = value;
    while(*src){

        if(*src!= '"'){

            *dst++=*src;

        }

        src++;

    }

    *dst = '\0';
    set_var(name, value);
    return 1;

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

    int toggle = 0;

    for (int i=0; input[i]!='\0'; i++){//we check if there are quotes

        if (input[i]=='"'){
            toggle=!toggle;
        }
        else if(input[i]=='|' && !toggle){
            return 1;
        }
    }
    return 0;
}

/*This function executes piped commands*/
void execute_pipes(char *input) {

    char **cmds = split_pipes(input);
    int n=0;

    while(cmds[n]){//count commands

        n++;

    }

    int pipefds[2 * (n - 1)];//create the pipes

    for(int i = 0; i < n - 1; i++){

        if(pipe(pipefds + i * 2) < 0){

            perror("pipe");//error handling
            exit(EXIT_FAILURE);

        }
    }

    for(int i=0; i< n; i++){

        pid_t pid=fork();

        if(pid == 0){//child

            if(i>0){//not the first

                redirect(pipefds[(i - 1) * 2], 0);
            
            }

            if(i<n-1){//not the last

                redirect(pipefds[i * 2 + 1], 1);

            }

            for(int j=0; j<2 * (n-1); j++){//closes all unused

                close(pipefds[j]);

            }

            char **args=tokenize(cmds[i]);//we get the args

            if(redirect_check(args)){

                handle_redirect(args);

            }

            execute(args);  // <- Important: handles built-ins and variables
            free(args);

            exit(0); // terminate child

        }
    }

    for(int i=0; i<2*(n-1); i++){//parent closes all

        close(pipefds[i]);

    }
    for (int i=0; i<n; i++){//waits for children

        wait(NULL);

    }

    for (int i = 0; i < n; i++){//free memory

        free(cmds[i]);

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
char **split_pipes(char *input) {

    char **cmds = malloc(64 * sizeof(char *));
    int count = 0;
    int toggle=0;//quote flag
    char *start=input;
    char *ptr=input;//current pos

    while(*ptr){//loop until end

        if(*ptr=='"'){//checks for quotes

            toggle = !toggle;

        }else if(*ptr=='|' && !toggle){

            *ptr = '\0';

            while(*start==' ' || *start=='\t'){//skip white space

                start++;

            } 
    
            char *end=ptr-1;

            while(end>start && (*end==' ' || *end== '\t')){//cut white space

                *end--='\0';

            } 

            cmds[count++] = strdup(start);
            start = ptr + 1;//next arg after |

        }
        ptr++;
    }

    while(*start==' ' || *start=='\t'){//skip white space

        start++;

    }

    if(*start!='\0'){//save final cmd

        cmds[count++]=strdup(start);

    }

    cmds[count] = NULL;//terminate
    return cmds;
}

/*This function splits commands by ; and \n (very similar to tokenize)*/
char **split_commands(char  *input){

    char **tokens=malloc(64 * sizeof(char*));//array of commands
    int toggle=0;//flag for quotes
    char *start=input;//start of current command
    int count=0;//num of commands

    if(!tokens){

        printf("Error\n");
        return NULL;

    }

    for(int i=0; input[i]!='\0'; i++){

        if(input[i]=='"'){//toggle quotes

            toggle=!toggle;

        }

        if((input[i]==';' || input[i]=='\n') && toggle==0){//if we need to split

            int len=input+i-start;//length of command
            char *cmd=malloc(len+1);
            strncpy(cmd, start, len);//copy command
            cmd[len]='\0';
            tokens[count]=cmd;//store command
            count++;
            start=input+i+1;//move start to next command

        }
    }

    if (*start != '\0'){//command without ; at the end

        int len = strlen(start);
        char *cmd = malloc(len + 1);
        strcpy(cmd, start);
        tokens[count++] = cmd;

    }

    tokens[count]=NULL;
    return tokens;

}

/*This function checks for redirections*/
int redirect_check(char **args){

    for(int i=0; args[i]!=NULL; i++){
        
        for(int j=0; args[i][j]!='\0'; j++){

            if(args[i][j]=='<' || args[i][j]=='>'){

                return 1;

            }
        }
    }

    return 0;

}

/*This function handles redirections*/
void handle_redirect(char **args){

    int in=-1;//input fd
    int out=-1;//output fd
    char *filename;

    for(int i=0; args[i]!=NULL;){//we check all arguments 

        if(strcmp(args[i], "<")==0){

            in=open(args[i+1], O_RDONLY);//read

            if(in<0){//error handling

                perror("Error opening file");
                exit(EXIT_FAILURE);

            }

            redirect(in,0);//we change the fd

            for(int j=i; args[j]!=NULL; j++){//we skip the 2 args

                args[j]=args[j+2];

            }

            continue;

        }else if(strcmp(args[i], ">")==0){//write

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

        }else if(strcmp(args[i], ">>")==0){//append

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

            i++;//only when its not a redirect

        } 
    }
}

/*Returns 1 if 2 strings match or 0 if they dont*/
int matches_key(char *line,char *key) {
    
    while(*line == ' ' || *line == '\t'){//we skip spaces

        line++;

    } 

    size_t key_len = strlen(key);

    for(size_t i = 0; i < key_len; i++){//compares characters

        if(line[i] != key[i]){
            return 0;
        }

    }

    
    for(size_t i= key_len; line[i] != '\0'; i++){//after matching it tests the rest

        if(line[i] != ' ' && line[i] != '\t' && line[i] != '\n' && line[i] != '\r'){

            return 0;

        }       
    }

    return 1;

}

/*This function handles flow*/
void handle_if(char *cmd){

    char **args;
    int found_then=-1;//index of then
    int found_fi=-1;//index of fi
    int status;
    char **cmds=split_commands(cmd);

    for(int i = 0; cmds[i] != NULL; i++){//searches for fi and then

        if(matches_key(cmds[i],"then")){

            found_then=i;

        }else if(matches_key(cmds[i],"fi")){

            found_fi = i;

        }
    }

    if(found_then == -1 || found_fi == -1){

        printf("Error: the syntax is wrong\n");
        return;

    }

    cmds[found_fi]=NULL;//terminate at fi

    char **condition=tokenize(cmds[0]);//with if
    
    status = execute(&condition[1]);//skips if and executes condition

    if(status==0){//if valid

        for(int i=2; cmds[i]!=NULL; i++){

            args=tokenize(cmds[i]);
            execute(args);
            free(args);

        }
    }

    for (int i = 0; cmds[i] != NULL; i++){

        free(cmds[i]);
    }

    free(cmds);

}

void handle_for(char *cmd){

    char **cmds=split_commands(cmd);
    int found_do=-1;//index of do
    int found_done=-1;//index of done
    char **tokens=tokenize(cmds[0]);//values
    
    for(int i=0; cmds[i]!= NULL; i++){//finds the indexes

        if(matches_key(cmds[i],"do")){

            found_do=i;

        }else if(matches_key(cmds[i],"done")){

            found_done=i;

        }

    }

    if(found_do == -1 || found_done == -1 || !matches_key(tokens[2],"in")){//error handling

        printf("Error: the syntax is wrong\n");
        return;

    }

    char* var=tokens[1];//the variable

    for(int i=3; tokens[i]!=NULL; i++){//loop

        set_var(var,tokens[i]);//we set the var

        for(int j = found_do + 1; j < found_done; j++){//for all commands inside do

            char *cmd_copy = strdup(cmds[j]);//copu because tokenize ruins things (spagheti code)
            char **args = tokenize(cmd_copy);   
            execute(args);                      
            free(args);
            free(cmd_copy);

        }
    }  
}

/*This function helps with flows and loop to gather the cmds*/
char *flow_gather(char *input,char *key){

    if(strstr(input, key)!=NULL){//if everything is in one line
        
        return strdup(input);

    }

    char *block = strdup(input);//copy of 1st line
    char line[1024];//buffer

    while(1){

        printf("> ");

        if(!fgets(line, sizeof(line), stdin)){//stops at EOF

            break;

        }

        block = realloc(block, strlen(block) + strlen(line) + 2);//we add the new line with extra space
        strcat(block, line);

        if (matches_key(line,key)){//if fi or done is typed we exit

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