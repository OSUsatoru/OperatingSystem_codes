/*
    ONID: yamamsat
    email: yamamsat@oregonstate.edu

    NOTE: I avoid to use single-line comment because this was created in cpp
    If I remember correctly, some of old compiler display error message for this.

    Here is the links where I checked while I was making Programming memo

    reference:

    - for some information
    cs344 Explorations

**********************************************************************************************/
/*
    memo
    it will be using modulo 27 operations: caputal letters and space


******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/*  This store the characters for key
strlen(key_characters) = 27
key = key_characters[ rand() % strlen(key_characters) ];
*/


char key_characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

void Generate_key(int length, char *str);

/*
    argc should be 2
    argv[0] = file path of keygen.exe
    argv[1] = keylength
************************************/
int main(int argc, char *argv[])
{
    /* If there is not enough argument */
    if(argc < 2){
        perror("You must provide the length of key\n");
        return EXIT_FAILURE;
    }

    /*set new sequence of psuedo random integers */
    srand((unsigned int)time(NULL));
    /* convert char to int */
    int key_length = atoi(argv[1]);

    /* allocate memory for key stirng, length +1 for NULL */
    char *key_string = calloc(key_length+1, sizeof(char));

    Generate_key(key_length, key_string);

    /* output to stdout */
    printf("%s\n", key_string);
    fflush(stdout);

    return EXIT_SUCCESS;
}

/*
    This function generates random characters of length
********************************************************/
void Generate_key(int length, char *str)
{
    for(int i = 0; i < length; ++i){
        str[i] = key_characters[ rand() % strlen(key_characters)];
    }
    /* add null character */
    str[length] = '\0';

}