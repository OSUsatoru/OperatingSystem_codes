#include <stdio.h>  // printf
#include <sys/types.h> // pid_t, not used in this example
#include <unistd.h> // getpid, getppid


int main(void) {

  printf("My pid is %d\n", getpid());
  printf("My parent's pid is %d\n", getppid());
  return 0;
}