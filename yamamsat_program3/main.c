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
    Todo:
        user input will be stored in struct
        1.skip space or "#"
        2.exit cd status or exec()
        use execlp() or execvp()
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

const int max_charactor = 2048;
const int max_arg = 512;


/*./testscript > mytestresults 2>&1*/
int main(void) {

    int x,y,n = 0;
    do{
        scanf("%d%d%d", &x,&y,&n);
        printf("result %d\n", (x+y));

        }while(n==0);

    return 0;
}