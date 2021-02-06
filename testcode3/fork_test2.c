
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PROCESS_NUM 3
#define ROOP_NUM 3


void child()
{
    pid_t c_pid;
    pid_t p_pid;
    int num;

    c_pid = getpid();
    p_pid = getppid();

    fprintf(stdout,"[PID:%d]start child process（parent's ID: %d）\n", c_pid, p_pid);

    for(num = 1; num <= ROOP_NUM; num++)
    {
        sleep(1);
        fprintf(stdout,"[PID:%d]%dloops\n", c_pid, num);
    }
    fprintf(stdout,"[PID:%d]end of child process\n",c_pid);
    return;
}

int main()
{
    pid_t c_pid;
    pid_t p_pid;
    int i;
    int status;


    p_pid = getpid();
    fprintf(stdout, "[PID:%d]start parent's process\n", p_pid);


    for(i = 0; i < PROCESS_NUM; i++)
    {
        switch(c_pid = fork())
        {
            case 0:
                child();
                exit(EXIT_SUCCESS);
            case -1:
                fprintf(stderr, "error \n");
                break;
            default:
                fprintf(stdout, "[PID:%d]child process(pid=%d)created\n", p_pid, c_pid);
                sleep(2);
                break;
        }
    }

    for(i = 0; i < PROCESS_NUM; i++)
    {
        c_pid = wait(&status);
        fprintf(stdout, "[PID:%d]child process(pid=%d)ends\n", p_pid, c_pid);
    }

    fprintf(stdout, "[PID:%d]parent process ends\n", p_pid);
    return 0;
}