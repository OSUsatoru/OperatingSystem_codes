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


/*./testscript > mytestresults 2>&1*/
int main(void) {

    char buffer[max_charactor];

    /*test for input data*/
    while(fgets(buffer, max_charactor-1, stdin)){
        printf("%s is %d\n",buffer, strlen(buffer));
        if (strlen(buffer) > 0 && buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer)-1] = '\0';
            }


        printf("%s is %d\n",buffer, strlen(buffer));
    }




    return 0;
}