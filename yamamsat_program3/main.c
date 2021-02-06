/*
    ONID: yamamsat
    email: yamamsat@oregonstate.edu

    NOTE: I avoid to use single-line comment because this was created in cpp
    If I remember correctly, some of old compiler display error message for this.

    reference:

    - for some information
    cs344 Explorations



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
    +1 for '\n'
*/
const int max_charactor = 2049;
const int max_arg = 512;


void process_input(char *input_args[], int *num_args);
void process_arguments(char *input_argus[]);

/*./testscript > mytestresults 2>&1*/
int main(void) {

    /*store all arguments in line*/
    char *input_args[max_arg];
    int num_args;
    /*test for input data*/
    do{
        process_input(input_args, &num_args);
        printf("num: %d\n", num_args);
        for(int i = 0; i < num_args; ++i)
            printf("%s", input_args[i]);
        printf("\n");
    }while(strcmp(input_args[0], "exit") != 0);




    return 0;
}
/*
    This receive input line by line and store each argument
    React to:
        1. "#" comment (no need)
        2. "&&" replace to pid
        3. ">" input file true -> dup2, 0
        4. ">", output file true -> dup2, 1

    1. return arguments
    2. return number of arguments

**********************************************************************/
void process_input(char *input_args[], int *num_args)
{
    char buffer[max_charactor];

    /* to store input*/
    fgets(buffer, max_charactor-1, stdin);

    /* replace the last charactore '\n' to '\0' */
    if (strlen(buffer) > 0 && buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer)-1] = '\0';
    }

    /* split string data into input_args*/
    int cnt = 0;
    /* for strtok_r*/
    char *saveptr;
    /* store title info */
    char *token = strtok_r(buffer, " ", &saveptr);

    while(token != NULL){
        input_args[cnt] = calloc(strlen(token)+1, sizeof(char));
        strcpy(input_args[cnt], token);
        ++cnt;
        token = strtok_r(NULL, " ", &saveptr);
    }
    *num_args = cnt;

}
/*
    This process the arguments
    1. exit: kill any other processes or jobs
    2. cd:
    3. status
******************************************************************/
void process_arguments(char *input_argus[])
{

}