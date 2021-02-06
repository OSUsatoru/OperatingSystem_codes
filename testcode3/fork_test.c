#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> // pid_t
#include <unistd.h> // fork

int main(){

	pid_t spawnpid = -5;
	int intVal = 10;
  	// If fork is successful, the value of spawnpid will be 0 in the child, the child's pid in the parent
	spawnpid = fork();
	printf("spawnpid: %d\n", spawnpid);
	printf("My pid is %d\n", getpid());
	printf("My parent's pid is %d\n", getppid());
	switch (spawnpid){
		case -1:
      	// Code in this branch will be exected by the parent when fork() fails and the creation of child process fails as well
			perror("fork() failed!");
			exit(1);
			break;
		case 0:
     	// spawnpid is 0. This means the child will execute the code in this branch
			intVal = intVal + 1;
			printf("I am the child! intVal = %d\n", intVal);
			break;
		default:
     	// spawnpid is the pid of the child. This means the parent will execute the code in this branch
			intVal = intVal - 1;
			printf("I am the parent! ten = %d\n", intVal);
			break;
	}
	printf("This will be executed by both of us!\n\n");
}
