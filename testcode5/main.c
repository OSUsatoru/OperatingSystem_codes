
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

char key_characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

void Is_Valid_Files(const char *text, const char *key);
/*
    argv[1]: plain text
    argv[2]: key test
    argv[3]: port numer
**************************************************/
int main(int argc, char *argv[])
{
    Is_Valid_Files(argv[1], argv[2]);
    return 0;
}
/*
    This function checks if the input text and key are valid.
    1. the size of key is begger than or equar to text
    2. text and key includes valid characters
*******************************************************/
void Is_Valid_Files(const char *text, const char *key)
{
    /* these store information for files */
    struct stat text_Stat;
    struct stat key_Stat;

    stat(text, &text_Stat);
    stat(key, &key_Stat);
    /*  these store the size of file by bytes
        On windows, the size of '\n' is 2 bytes
        there might be difference between this size and strlen(text),
        but this size is always size >= strlen(text)
    *********************************************************************/
    unsigned long text_size = text_Stat.st_size, key_size = key_Stat.st_size;

    printf("%d\n%d\n", text_size, key_size);


    /* open files */
    FILE *text_file = fopen(text,"r");
    FILE *key_file = fopen(key,"r");

    /* if faild to open file */
    if(text_file == NULL){
        fprintf(stderr,"Error: Cannot open %s\n", text);
        exit(1);
    }else if(key_file == NULL){
        fprintf(stderr,"Error: Cannot open %s\n", key);
        exit(1);
    }

    /* Allocate memory by using size of file*/
    char *text_buffer = malloc(text_size);
    char *key_buffer = malloc(key_size);
    /* Store string including '\n' */
    fgets(text_buffer, text_size, text_file);
    fgets(key_buffer, key_size, key_file);
    /* Close the files */
    fclose(text_file);
    fclose(key_file);
    /* Updata size of file */
    text_size = strlen(text_buffer)-1;
    key_size = strlen(key_buffer)-1;

    if(text_size > key_size){
        /* free the memory and return 0*/
        free(text_buffer);
        free(key_buffer);
        fprintf(stderr,"Error: key '%s' is too short\n", key);
        exit(1);
    }

    /* Replace '\n' to NULL character */
    text_buffer[strlen(text_buffer)] = '\0';
    key_buffer[strlen(key_buffer)] = '\0';
    printf("%s:%d\n", text_buffer, text_size);
    printf("%s:%d\n", key_buffer, key_size);

    /*
        This is not efficient though
    ************************************************/
    for(int i = 0; i < text_size; ++i){
        /* If text has a bad character */
        if(strchr(key_characters, text_buffer[i]) == NULL){
            /* free the memory and return 0*/
            free(text_buffer);
            free(key_buffer);
            fprintf(stderr,"Error: '%s' has a bad character\n", text);
            exit(1);
        }
    }
    for(int i = 0; i < key_size; ++i){
        /* If key has a bad character */
        if(strchr(key_characters, key_buffer[i]) == NULL){
            /* free the memory and return 0*/
            free(text_buffer);
            free(key_buffer);
            fprintf(stderr,"Error: '%s' has a bad character\n", key);
            exit(1);
        }
    }

    /* free the memory and return 0*/
    free(text_buffer);
    free(key_buffer);

}