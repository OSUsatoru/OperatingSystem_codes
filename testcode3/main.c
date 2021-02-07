#include <stdio.h>  // printf
#include <sys/types.h> // pid_t, not used in this example
#include <unistd.h> // getpid, getppid
#include <string.h>

int main(void) {

  char str[10];
  fgets(str, 10, stdin);
  printf("%d\n", strlen(str));
  return 0;
}