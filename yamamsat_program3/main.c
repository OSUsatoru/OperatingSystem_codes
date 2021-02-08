/*
    ONID: yamamsat
    email: yamamsat@oregonstate.edu

    NOTE: I avoid to use single-line comment because this was created in cpp
    If I remember correctly, some of old compiler display error message for this.

    reference:

    - for some information
    cs344 Explorations
    - for replacing charactor
    https://mogachan.net/clangmemcpy/
    and
    https://programming-place.net/ppp/contents/c/rev_res/string014.html
    - for the reason fo using fflush
    https://daeudaeu.com/fflush/


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
#include <sys/wait.h>
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

/* reset for new input */
struct arst{
    int num_arg;
    _Bool backgroundCommand;
    char *InputFile;
    char *OutputFile;
};
/* unchanged status*/
struct gloval_arst{
    _Bool active_SIGTSTP;
    int exit_status;
};

/* list of bg_process */
/*
struct bg_process{
    pid_t bg_processes;
    struct bg_process *next;
};*/
/* this is not efficient */
/*int bg_process[128];*/
int num_bg_process = 0;
/*
struct bg_process *head = NULL;
struct bg_process *tail = NULL;
*/

void check_bgprocess(struct gloval_arst *glar);

void input_argument(char *input_args[], struct arst *arg_status);
int process_arguments(char *input_argument[], struct arst *arg_status, struct gloval_arst *glar);
void execute_command(char *input_argument[], struct arst *arg_status, struct gloval_arst *glar);

/*
    There variable are chenged by each input
************************************************************/


/*./testscript > mytestresults 2>&1*/
int main(void) {

    /*store all arguments in line*/
    char *input_args[max_arg];
    int isContinue = 1;
    /* initialize all elements */
    /*memset(bg_process, -1, sizeof(bg_process));*/

    struct gloval_arst gloval_status = {0, 1};

    do{
        //printf("check: %d", arg_status->check);
        struct arst arg_status = {0,0,NULL,NULL};

        //arg_status->check = 1;
        input_argument(input_args, &arg_status);
        //printf("num: %d\n", num_args);

        /*check if the argument is comment or empty*/
        if(input_args[0][0] != 35 && arg_status.num_arg!= 0){
            isContinue = process_arguments(input_args, &arg_status, &gloval_status);
            if(isContinue == 2){
                /*use execvp*/
                /*
                    my pc
                    execvp(input_args[0], const char * const*(input_args));
                    or  on os server
                    execvp (const char *__file, char *const __argv[])
                */
                execute_command(input_args, &arg_status, &gloval_status);
                //execvp(input_args[0], (const char * const*)input_args);
            }
        }

        /*
        for(int i = 0; i < arg_status.num_arg; ++i){
            printf("%s ", input_args[i]);
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
        */
        /* reset the argument array*/


        /* reset all arguments*/
        for(int i = 0; i < arg_status.num_arg; ++i){
            input_args[i] = NULL;
        }
        check_bgprocess(&gloval_status);

    }while(isContinue);




    return 0;
}
/*
    check the back ground process
    if the process is terminated, display message and delete it from list
****************************************************************************/
void check_bgprocess(struct gloval_arst *glar)
{
        int spawnPid;
        //int cnt = num_bg_process;
		while ( (spawnPid = waitpid(-1, &(glar->exit_status), WNOHANG)) > 0){
			printf("background pid %d terminated is done: ", spawnPid);
                fflush(stdout);

            /*normal*/
            if (WIFEXITED(glar->exit_status)){
                printf("exit value %d\n", WEXITSTATUS(glar->exit_status));
                fflush(stdout);
			} /*abnormal*/
            else if(WIFSIGNALED(glar->exit_status)){
                printf("terminated by signal %d", WTERMSIG(glar->exit_status));
                fflush(stdout);
            }
        }

        /*
        for(int i = 0; i < 128; ++i){
            if(bg_process[i] == -1){
                break;
            }else if(waitpid(bg_process[i], &(glar->exit_status) , WNOHANG) > 0){
                printf("background pid %d terminated is done: ", bg_process[i]);
                fflush(stdout);
            }

            if (WIFEXITED(glar->exit_status)){
                printf("exit value %d\n", bg_process[i], WEXITSTATUS(glar->exit_status));
                fflush(stdout);
			}
            else if(WIFSIGNALED(glar->exit_status)){
                printf("terminated by signal %d", WTERMSIG(glar->exit_status));
                fflush(stdout);
            }
            bg_process[i] = -1;
        }
        */

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


    /* getpid()*/
    int num_pid = getpid();
    char pid[32];
    /* for replacment, convert PID to string*/
    sprintf(pid, "%d", num_pid);
    char *tmp = buffer;
    /*
        if there is "$$", replace that to PID
    */
    while((tmp = strstr(tmp, "$$")) != NULL){
        /* tmp2 is pointing after "$$" */
        const char *tmp2 = tmp + 2;
        /* move tmp2 after tmp + strlen(pid) */
        memmove(tmp + strlen(pid), tmp2, strlen(tmp2)+1);
        /* cpy pid to tmp */
        memcpy(tmp, pid, strlen(pid));
        /* next pointer */
        tmp+=strlen(pid);
    }

    /* split string data into input_args*/
    int cnt = 0;
    /* for strtok_r*/
    char *saveptr;
    /* store title info */
    char *token = strtok_r(buffer, " ", &saveptr);

    /* check if there is: ">" "<" */
    while(token != NULL){


        input_args[cnt] = calloc(strlen(token)+1, sizeof(char));
        /*
            if token == ">" or "<", store next file name into struct arst. (do not store into input_args)
            if token == "&", arst->backgroundCommand = 1. (do not store into input_args)
            if token has the
        */
        if(!strcmp(token,"<")){
            /* move to next argument to store file name*/
            token = strtok_r(NULL, " ", &saveptr);

            /* dinamic allocate memory for title */
            arg_status->InputFile = calloc(strlen(token)+1, sizeof(char));
            strcpy(arg_status->InputFile, token);

        }else if(!strcmp(token,">")){
            /* move to next argument to store file name*/
            token = strtok_r(NULL, " ", &saveptr);

            /* dinamic allocate memory for title */
            arg_status->OutputFile = calloc(strlen(token)+1, sizeof(char));
            strcpy(arg_status->OutputFile, token);

        }else if(!strcmp(token, "&")){
            /* backgroundcommand = 1 */
            arg_status->backgroundCommand = 1;
        }else{

            input_args[cnt] = calloc(strlen(token)+1, sizeof(char));
            strcpy(input_args[cnt], token);
            ++cnt;
        }

        token = strtok_r(NULL, " ", &saveptr);
    }

    /* store number of arguments on struct*/
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
int process_arguments(char *input_argus[], struct arst *arg_status, struct gloval_arst *glar)
{
    /*
    printf("%s\n", input_argus[0]);
    fflush(stdout);
    */
   /*
    if(arg_status->backgroundCommand){
        printf("background true\n");
        fflush(stdout);
    }
    if(arg_status->InputFile != NULL){
        printf("Input true: %s\n", arg_status->InputFile);
        fflush(stdout);
    }
    if(arg_status->OutputFile != NULL){
        printf("output true: %s\n", arg_status->OutputFile);
        fflush(stdout);
    }
    */

    /* these are built in arguments, no need to check if they are on background process*/
    /* exit: kill any other processes or jobs and return 0*/
    if(!strcmp(input_argus[0],"exit")){
        return 0;
    }
    /*
        cd: use chdir
            return value: 0 (success), 1(error)
        if argument is 1, go home
        if 2 arguments ,
    */
    else if(!strcmp(input_argus[0],"cd")){
        /* change dir to home*/
        if(arg_status->num_arg == 1){
            chdir(getenv("HOME"));
        }
        /* try to change current working dir */
        else if(arg_status->num_arg == 2){
            char currentDir[128], newPath[128];
            int chdir_check;
            /* try 1: absolute path*/
            chdir_check = chdir(input_argus[1]);
            /* if try 1 faild, check current dir */
            if(chdir_check){
                getcwd(currentDir, 128);
                sprintf(newPath, "%s/%s", currentDir, input_argus[1]);
                chdir_check = chdir(newPath);
            }
            /* if both faild*/
            if(chdir_check){
                printf("cd command faild.\n");
                fflush(stdout);
            }
        }
        else{
            printf("Too much argument for cd command\n");
            fflush(stdout);
        }

        return 1;
    }else if(!strcmp(input_argus[0],"status")){
        if(WIFEXITED(glar->exit_status)){
            printf("exit value %d\n", WEXITSTATUS(glar->exit_status));
            fflush(stdout);
		} /*abnormal*/
        else if(WIFSIGNALED(glar->exit_status)){
            printf("terminated by signal %d", WTERMSIG(glar->exit_status));
            fflush(stdout);
        }
        return 1;
    }
    else{
        return 2;
    }
}
/*
    This executes other commands
    if there is input or output file, try open and redirect stdout and stdin
    if backgroundCommand = 1 and  active_SIGTSTP = 0, do not need to wait for child
    else need to wait for child
**************************************************************************/
void execute_command(char *input_argument[], struct arst *arg_status, struct gloval_arst *glar)
{
    int in_fdescriptor = 0, out_fdescriptor = 0;
    int result;
    pid_t spawnPid = -5;

    spawnPid = fork();
	switch (spawnPid) {
		case -1:
			perror("fork() failed!");
			exit(1);
			break;
		case 0:
            /* if need to redirect input output */
			if(arg_status->InputFile != NULL){
                in_fdescriptor = open(arg_status->InputFile, O_RDONLY);
                /* if failed open file*/
				if (in_fdescriptor == -1){
					printf("cannot open %s for input\n", arg_status->InputFile);
                    fflush(stdout);
					exit(1);
				}
                result = dup2(in_fdescriptor, 0);
                /* if dup2 failed*/
				if (result == -1){
					perror("dup2()\n");
					exit(2);
				}
            }
            if(arg_status->OutputFile != NULL){
                out_fdescriptor = open(arg_status->OutputFile, O_CREAT|O_WRONLY|O_APPEND, 0666);
                /* if failed open file*/
				if (out_fdescriptor == -1){
					printf("cannot open %s for output\n", arg_status->OutputFile);
                    fflush(stdout);
					exit(1);
				}
                result = dup2(out_fdescriptor, 1);
                /* if dup2 failed*/
				if (result == -1){
					perror("dup2()\n");
					exit(2);
				}
            }



			execvp(input_argument[0], (char* const*)input_argument);

            /*error if child reachs here*/
			printf("%s: no such file or directory\n", input_argument[0]);
            fflush(stdout);
            /*close file*/
            close(in_fdescriptor);
            close(out_fdescriptor);
			exit(1);


            break;

		default:

            /* if there was & and SIGTSTP is not active*/
			if (arg_status->backgroundCommand && glar->active_SIGTSTP == 0) {
                /* no need to wait for this child */
				pid_t childPid = waitpid(spawnPid, &(glar->exit_status) , WNOHANG);
				printf("background pid is %d\n", spawnPid);
				fflush(stdout);
			}
			// Otherwise execute it like normal
			else {
				pid_t childPid = waitpid(spawnPid, &(glar->exit_status), 0);
			}

	}
}