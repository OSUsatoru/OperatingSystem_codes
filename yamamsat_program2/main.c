/*
    ONID: yamamsat
    email: yamamsat@oregonstate.edu

    NOTE: I avoid to use single-line comment because this was created in cpp
    If I remember correctly, some of old compiler display error message for this.

    reference:

    - for some information
    cs344 Explorations

    - fixing error for mkdir on windows
    https://stackoverflow.com/questions/19479746/implicit-declaration-of-mkdir
    https://stackoverflow.com/questions/10356712/mkdir-windows-vs-linux
    - for the reason of including sys/types.h
    https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_stat.h.html
    - for sprintf and fprintf
    https://hikotech.net/post-331/
    - for scanf error
    https://programming.pc-note.net/c/scanf.html

**********************************************************************************************/
/*
    "A conforming C-language application must include <sys/stat.h> for functions
    that have arguments or return values of type mode_t,
    so that symbolic values for that type can be used. An alternative would be to require
    that these constants are also defined by including <sys/types.h>."
    - (The Open Group Base Specifications Issue 7, 2018 edition
      IEEE Std 1003.1-2017 (Revision of IEEE Std 1003.1-2008)
      Copyright © 2001-2018 IEEE and The Open Group)
*********************************************************************************************/
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

/*on windows, I need to include this
  on the osu engr server(Linux), this is not necessery
  error message: warning: implicit declaration of function 'mkdir
  Therefore, create function prototype of mkdir here
**************************************************************/
/*int mkdir(const char *pathname, mode_t mode);*/
#ifdef _WIN32
    # define  mkdir( D, M )   _mkdir( D )
#endif

typedef unsigned long long int ull;

void solve();

char *Find_Max_Min_File(int input);
_Bool End_With_Target(const char* str, const char* target);
_Bool Start_With_Target(const char* str, const char* target);
_Bool Find_file(char *FileName);
void Create_Movie_Directory(char *FileName);
void Create_Movie_Files(char *filePath, char *dirPath);
struct movies *get_movie_info(char *current_Line);

/*
    this structure is for movie file
    file_size is unsigned long long
    unsigned long long: 0 to 18,446,744,073,709,551,615 (e19)
    unsigned long might be enough
****************************************************/
struct movies{
    char *title;
    int year;
    char *languages;
    double rating;
    struct movies *next;
};

int main(int argc, char *argv[])
{
    /*set new sequence of psuedo random integers */
    srand((unsigned int)time(NULL));

    int choice,cnt;
    /*loop while user input == 2*/
    do{
        printf("1. Select file to process\n");
        printf("2. Exit the program\n\n");
        printf("Enter a choice 1 or 2: ");
        cnt=scanf("%d%*c", &choice);
        switch(choice){
            case 1:
                solve();
                break;
            case 2:
                break;
            default:
                printf("You entered an incorrect choice. Try again.\n\n");
                /*when user input is not numerical input*/
                if(cnt!=1){
                    printf("error\n");
                    scanf("%*[^\n]");
                    scanf("%*c");
                }

                break;
        }
    }while(choice != 2);

    return EXIT_SUCCESS;
}
/*
    This function is for user input 1 in first phase
    the process is corresponding to user input:
    Enter 1 to pick the largest file
    Enter 2 to pick the smallest file
    Enter 3 to specify the name of a file
*****************************************************************************/
void solve()
{
    /* 1 when user input is invalid */
    int invalid = 1;

    /*loop while valid input*/
    do{
        int choice, cnt;
        printf("\nWhich file you want to process?\n");
        printf("Enter 1 to pick the largest file\n");
        printf("Enter 2 to pick the smallest file\n");
        printf("Enter 3 to specify the name of a file\n\n");
        printf("Enter a choice from 1 to 3: ");
        /*Get user input for choice*/
        cnt = scanf("%d%*c", &choice);
        char *fileName;
        char user_file_name[256], empty_file[]="Cannot find file";
        switch(choice){
            case 1: case 2:
                fileName = Find_Max_Min_File(choice);
                /*If there in not valid file in current directory*/
                if(!strcmp(fileName,empty_file)){
                    printf("Cannot find valid file data in current directory.\n\n");
                }else{
                    Create_Movie_Directory(fileName);
                }
                invalid = 0;
                break;
            case 3:
                printf("Enter the complete file name: ");
                /*Get user input for file name*/
                scanf("%s%*c", user_file_name);
                /*If there is a file, creaate directory*/
                if(Find_file(user_file_name)){
                    Create_Movie_Directory(user_file_name);
                    invalid=0;
                }else{
                    printf("The file %s was not found. Try again\n", user_file_name);
                    invalid=1;
                }
                break;
            default:
                printf("You entered an incorrect choice. Try again.\n");
                /*when user input is not numerical input*/
                if(cnt!=1){
                    scanf("%*[^\n]");
                    scanf("%*c");
                }
                invalid=1;
                break;
        }
    }while(invalid);

}
/*
    This function return csv file name that starts with movies_ in current directory
    case 1: maxinum file size
    case 2: minimum file size
    if there is not valid file in current directory, return "Cannot find file";
*****************************************************************************/
char *Find_Max_Min_File(int input)
{
    /*open directory*/
    DIR *currDir = opendir(".");
    /*these stores information for directory and file*/
    struct dirent *aDir;
    struct stat dirStat;

    int cnt = 0;
    char *empty_file="Cannot find file";
    char filePath[256];
    /*These stores the max and min file name*/
    char *maxFileName, *minFileName;
    /*These stores the max and min file size*/
    unsigned long maxFileSize = 0, minFileSize = ULONG_MAX;

    /* ends when reading all files in current directory(currDir)*/
    while((aDir=readdir(currDir)) != NULL){

        /* check if the file is valid*/
        if(Start_With_Target(aDir->d_name,"movies_") && End_With_Target(aDir->d_name,".csv")){
            ++cnt;
            sprintf(filePath, "./%s", aDir->d_name);
            stat(filePath, &dirStat);
            /*if the file is the largest*/
            if(dirStat.st_size >= maxFileSize){
                maxFileSize=dirStat.st_size;
                maxFileName = calloc(strlen(aDir->d_name)+1, sizeof(char));
                strcpy(maxFileName, aDir->d_name);
            }
            /*if the file is the smallest*/
            if(dirStat.st_size <= minFileSize){
                minFileSize=dirStat.st_size;
                minFileName = calloc(strlen(aDir->d_name)+1, sizeof(char));
                strcpy(minFileName, aDir->d_name);
            }
        }
    }
    /*
    printf("max: %lu\nmin: %lu\n", maxFileSize, minFileSize);
    printf("max: %s\nmin: %s\n", maxFileName, minFileName);
    */

    /*If there is no valid file*/
    if(cnt==0){
        return empty_file;
    }
    if (input == 1){
        return maxFileName;
    }else{
        return minFileName;
    }
}
/*
    This checks if the str ends with target
    return 1 if true
    else 0
******************************************************************/
_Bool End_With_Target(const char* str, const char* target)
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
/*
    This checks if the str starts with target
    return 1 if true
    else 0
******************************************************************/
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
/*
    This checks if the file is in current directory
    return 1 if true
    else 0
********************************************************************/
_Bool Find_file(char *FileName)
{
    /*open directory*/
    DIR *currDir = opendir(".");
    /*these stores information for directory*/
    struct dirent *aDir;
    while((aDir=readdir(currDir)) != NULL){
        if(!strcmp(FileName, aDir->d_name)){
            return 1;
        }
    }
    return 0;
}
/*
    This function receive the File name and creates directory with
    random number(0 to 99999) rand% 100000
    also creates txt files inside that directory.

    MODE:
    directory: rwx r-x --- ==  0750

    NOTE: I assume that 100000 directories are enough for this assignment, and
          if I generate the same name of directory, I am going to generate new
          random number until I can create new name of directory;

          IF THERE ARE DIRECTORIES WITH 0 TO 100000, MY PROGRAM WILL CAUSE HUGE PROBLEM
          (THAT IS CONTROLLED BY cnt < 100000 for safty)
*****************************************************************/
void Create_Movie_Directory(char *FileName)
{
    printf("Now processing the chosen file named %s\n",FileName);
    /*0 to 99999*/
    char dirName[256], dirPath[256], filePath[256];
    /*This will stop infinite loop that may happen*/
    int cnt = 0;

    /* loop until it can create new directory or cnt is 100000*/
    do{
        int r_num = rand()%100000;
        sprintf(dirName,"yamamsat.movies.%d",r_num);
        ++cnt;
    }while(mkdir(dirName, 0750) && cnt < 100000);

    /*Error case*/
    if(cnt >= 100000){
        printf("We cannot create directory anymore\n");
        return;
    }

    printf("Created directory with name %s\n\n", dirName);
    sprintf(dirPath,"./%s", dirName);
    sprintf(filePath,"./%s", FileName);
    Create_Movie_Files(filePath, dirPath);

}
/*
    This function receives file name and directory path
    Read the file and create txt files by that
    (Only user title and year data)
    MODE:
    file: rw- r-- --- == 0640
**********************************************************/
void Create_Movie_Files(char *filePath, char *dirPath)
{
    /*open csv file for only read*/
    FILE *movieFile = fopen(filePath, "r");

    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    /* do need to store info of the first line */
    getline(&currLine, &len, movieFile);

    char year_filePath[256], WR_message[256];
    /* Read the file line by line and store information into the file by year of movie*/
    while ((nread = getline(&currLine, &len, movieFile)) != -1){
        int fdescriptor = 0;

        struct movies *movie_info = get_movie_info(currLine);
        /*printf("title:%s\nyear:%d\n", movie_info->title, movie_info->year);*/
        sprintf(year_filePath, "%s/%d.txt", dirPath, movie_info->year);

        /*Open file for only write. Create file if there is not exist.
          Every write operation to append to the end of the file  MODE: 0640*/
        fdescriptor = open(year_filePath, O_CREAT|O_WRONLY|O_APPEND, 0640);

        /*error case*/
        if (fdescriptor == -1){
            printf("open() failed on \"%s\"\n", year_filePath);
            perror("Error");
            exit(1);
        }
        /*append the message to the end of the file*/
        sprintf(WR_message, "%s\n", movie_info->title);
        int nwrite = write(fdescriptor,WR_message, strlen(WR_message));
        /*Close file*/
        close(fdescriptor);
    }
    /* should free the memory that is never used again */
    free(currLine);
    /*close file*/
    fclose(movieFile);
}
/*
    This function receives the string information
    and return struct movies
*************************************************/
struct movies *get_movie_info(char *current_Line)
{
    struct movies *input_movie = malloc(sizeof(struct movies));

    /* for strtok_r */
    char *saveptr;
    /* store title info */
    char *token = strtok_r(current_Line, ",", &saveptr);
    /* dinamic allocate memory for title */
    input_movie->title = calloc(strlen(token)+1, sizeof(char));
    /* copy token info to title */
    strcpy(input_movie->title, token);


    /* store year info */
    token = strtok_r(NULL, ",", &saveptr);
    input_movie->year = atoi(token);

    /* store languages info */
    token = strtok_r(NULL, ",", &saveptr);
    /* want to copy token from token's index 1 and store number of size-2 characters */
    input_movie->languages = calloc(strlen(token)-1, sizeof(char));
    strncpy(input_movie->languages,token+1, strlen(token)-2);
    /* the last charactor is '\0 */
    input_movie->languages[strlen(token)-2] = '\0';

    /* store rating info */
    token = strtok_r(NULL, "\n", &saveptr);
    input_movie->rating = strtod(token, NULL);
    /* store next info */
    input_movie->next = NULL;

    return input_movie;
}