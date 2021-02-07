/*
    ONID: yamamsat
    email: yamamsat@oregonstate.edu

    NOTE: I avoid to use single-line comment because this was created in cpp
    If I remember correctly, some of old compiler display error message for this.

    reference:

    - for some information
    cs344 Explorations
    - for replacing charactor
    https://programming-place.net/ppp/contents/c/rev_res/string014.html



**********************************************************************************************/
/*
    no need to use getline
    user fgets, not scanf (need to store data line by line)
    have to delete the last charactor
    Todo:
        user input will be stored in struct
        1.skip space or "#"
        2.exit cd status or exec()
        use execlp() or execvp()

        if "<", input file true -> dup2, 0
        if ">", output file true -> dup2, 1
        dup2

        if "#", comment true
        if "&", background process true
        if "$$", replace that to pid

**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*
    fgets automatically stores '\n' at the end of string.
    the end of buffer, stores '\0'
    +1 for max_charactor
*/
const int max_charactor = 2049;
/*
    because the exec function's argu has to end with NULL
    +1 for max_arg
*/
const int max_arg = 513;

struct arst{
    int num_arg;
    _Bool backgroundCommand;
    char *InputFile;
    char *OutputFile;
};


void input_argument(char *input_args[], struct arst *arg_status);
int process_arguments(char *input_argus[]);

/*
    There variable are chenged by each input
************************************************************/


/*./testscript > mytestresults 2>&1*/
int main(void) {

    /*store all arguments in line*/
    char *input_args[max_arg];
    int isContinue = 1;


    do{
        //printf("check: %d", arg_status->check);
        struct arst arg_status = {0};

        //arg_status->check = 1;
        input_argument(input_args, &arg_status);
        //printf("num: %d\n", num_args);

        /*check if the argument is comment*/
        if(input_args[0][0] != 35 && arg_status.num_arg!= 0){
            isContinue = process_arguments(input_args);
        }

        /*
        for(int i = 0; i < num_args; ++i)
            printf("%s", input_args[i]);
        printf("\n");
        */

        /* reset the argument array*/
        for(int i = 0; i < arg_status.num_arg; ++i){
            input_args[i] = NULL;
        }
    }while(isContinue);




    return 0;
}
/*
    This receive input line by line and store each argument
    React to:
        1. "#" comment (no need)

        2. "&&" replace to pid
        3. "<" input file true -> dup2, 0
        4. ">", output file true -> dup2, 1

    1. return arguments
    2. return arg_status

**********************************************************************/
void input_argument(char *input_args[],  struct arst *arg_status)
{
    char buffer[max_charactor];
    /* Display ": " when store argument
    printf(": ");
    fflush(stdout);
    */

    /* to store input*/
    fgets(buffer, max_charactor-1, stdin);

    /* replace the last charactore '\n' to '\0' */
    if (strlen(buffer) > 0 && buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer)-1] = '\0';
    }

    /* if the input is blank*/
    if(!strcmp(buffer, "")){
        input_args[0] = "";
        arg_status->num_arg = 0;
        return;
    }




    /* split string data into input_args*/
    int cnt = 0;
    /* for strtok_r*/
    char *saveptr;
    /* store title info */
    char *token = strtok_r(buffer, " ", &saveptr);

    /* check if there is: ">" "<" */
    while(token != NULL){


        char *cpy_token = calloc(strlen(token)+1, sizeof(char));
        strcpy(cpy_token, token);
        char pid[] = "123";
        char *tmp = cpy_token;
        char tmp_buffer[max_charactor];
        /*
            if there is "$$", replace that to PID
        */
        while((tmp = strstr(tmp, "$$")) != NULL){
            /* tmp2 is pointing after "$$" */
            const char *tmp2 = tmp + 2;
            memmove(tmp + strlen(pid), tmp2, strlen(tmp2)+1);
            memcpy(tmp, pid, strlen(pid));
            tmp+=strlen(pid);
        }


        input_args[cnt] = calloc(strlen(cpy_token)+1, sizeof(char));
        /*
            if token == ">" or "<", store next file name into struct arst. (do not store into input_args)
            if token == "&", arst->backgroundCommand = 1. (do not store into input_args)
            if token has the
        */
        if(!strcmp(cpy_token,"<")){
            /* move to next argument to store file name*/
            cpy_token = strtok_r(NULL, " ", &saveptr);

            /* dinamic allocate memory for title */
            arg_status->InputFile = calloc(strlen(cpy_token)+1, sizeof(char));
            strcpy(arg_status->InputFile, cpy_token);

        }else if(!strcmp(cpy_token,">")){
            /* move to next argument to store file name*/
            cpy_token = strtok_r(NULL, " ", &saveptr);

            /* dinamic allocate memory for title */
            arg_status->OutputFile = calloc(strlen(cpy_token)+1, sizeof(char));
            strcpy(arg_status->InputFile, cpy_token);

        }else if(!strcmp(cpy_token, "&")){
            /* backgroundcommand = 1 */
            arg_status->backgroundCommand = 1;
        }else{

            input_args[cnt] = calloc(strlen(cpy_token)+1, sizeof(char));
            strcpy(input_args[cnt], cpy_token);
            ++cnt;
        }


        token = strtok_r(NULL, " ", &saveptr);
    }
    arg_status->num_arg = cnt;

    /* add NULL for execvp*/
    input_args[cnt] = NULL;

}
/*
    This process the arguments
    1. exit: kill any other processes or jobs and return 0
    2. cd:
    3. status
    4. other
******************************************************************/
int process_arguments(char *input_argus[])
{
    printf("%s\n", input_argus[0]);
    if(!strcmp(input_argus[0],"exit")){
        return 0;
    }else{
        return 1;
    }
}