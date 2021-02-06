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

#ifdef _WIN32
    # define  mkdir( D, M )   _mkdir( D )
#endif

int find_first_string(const char* str, const char* target)
{
    if( !(str-strstr(str,target)) ){
        return 1;
    }else{
        return 0;
    }

}
_Bool find_last_string(const char* str, const char* target)
{
    /*length of str and target*/
    size_t str_len = strlen(str), target_len = strlen(target);

    /*target is longer than str*/
    if(str_len < target_len){
        return 0;
    }else{
        /*index where we start to search*/
        size_t index = str_len-target_len;
        /*check charactors between index to last*/
        if(!strncmp(&str[index], target, target_len)){
            return 1;
        }else{
            return 0;
        }
    }

}

_Bool Start_With_Target(const char* str, const char* target)
{
    /*length of str and target*/
    size_t str_len = strlen(str), target_len = strlen(target);

    /*target is longer than str*/
    if(str_len < target_len){
        return 0;
    }
    /*compare first target_len of charactors*/
    if(!strncmp(str,target, target_len)){
        return 1;
    }else{
        return 0;
    }
}

void display_files(char *dir)
{
    DIR *currDir = opendir(dir);
    struct dirent *aDir;
    struct stat dirStat;


    while((aDir = readdir(currDir)) != NULL){
        stat(aDir->d_name, &dirStat);
        printf("name: %s: size: %d \n", aDir->d_name, dirStat.st_size);

        DIR *inDir = opendir(aDir->d_name);
        struct dirent *bDir;
        while((bDir = readdir(inDir))!=NULL){

            char filePath[256];
            sprintf(filePath, "%s/%s", aDir->d_name, bDir->d_name);
            stat(filePath, &dirStat);

            stat(bDir->d_name, &dirStat);
            printf("%s->%s: (%s) size: %ld \n",aDir->d_name, bDir->d_name, filePath, dirStat.st_size);

        }
    }
}

int main(int argc, char *argv[])
{
    char year_filePath[] = "./check/input.txt";
    int fdescriptor = 0;

    fdescriptor = open(year_filePath, O_CREAT|O_WRONLY|O_APPEND, 0640);
    char mess[] = "testing";

    if (fdescriptor == -1){
        printf("open() failed on \"%s\"\n", year_filePath);
        perror("Error");
        exit(1);
    }
    int nwrite = write(fdescriptor, mess, strlen(mess));

    close(fdescriptor);
    /*
    char str[]="movies_a.csv.csv";
    char *test;

    if(Start_With_Target(str,"movies_") && find_last_string(str,".csv")){
        printf("ok\n");
    }

    DIR *currDir = opendir(".");

    struct dirent *aDir;
    struct stat dirStat;


    while((aDir=readdir(currDir)) != NULL){
        if(Start_With_Target(aDir->d_name,"movies_") && find_last_string(aDir->d_name,".csv")){
            printf("name: %s\n", aDir->d_name);
        }
    }*/


    return EXIT_SUCCESS;
}