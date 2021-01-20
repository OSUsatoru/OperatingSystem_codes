/*
    ONID: yamamsat
    email: yamamsat@oregonstate.edu

    NOTE: I avoid to use single-line comment because this was created in cpp
    If I remember correctly, some of old compiler display error message for this.

    Here is the links where I checked while I was making Programming memo

    reference:
    - fixing error for mkdir on windows
    https://stackoverflow.com/questions/19479746/implicit-declaration-of-mkdir
    - for the reason of including sys/types.h
    https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_stat.h.html
**********************************************************************************************/
/*
    Programming memo:

******************************************************************************************/
/* To do
    read every file information and size of them
    do not need to sort these information by size.
    track max size and min size of files
    directory: rwx r-x --- ==  0750
    file: rw- r-- --- == 0640

*/
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
#include <sys/stat.h>
#include <sys/types.h>
typedef unsigned long long int ull;

/*on windows, I need to include this
  on the osu engr server(Linux), this is not necessery
  error message: warning: implicit declaration of function 'mkdir
  Therefore, create function prototype of mkdir here
**************************************************************/
int mkdir(const char *pathname, mode_t mode);
struct movies *createMovie(char *current_Line);
struct movies *processFile(char *filePath);

/*
    this structure is for movie file
    file_size is unsigned long long
    unsigned long long: 0 to 18,446,744,073,709,551,615 (e19)
    unsigned long might be enough
****************************************************/
struct movies{
    ull file_size;
    char *title;
    int year;
    char *languages;
    double rating;
    struct movies *next;
};

int main(int argc, char *argv[])
{
    /* array of all input files information*/
    struct movies *movie_list = calloc(sizeof(struct movies), argc-1);
    /*
    char file_name1[256],file_name2[256];
    if (!mkdir(argv[1], 7)) {
      printf("%s has created\n", argv[1]);
    }
    sprintf(file_name1,"./%s/text.txt", argv[1]);
    sprintf(file_name2,"./%s//text2.txt", argv[1]);
    FILE *fp1 = fopen(file_name1, "w");
    FILE *fp2 = fopen(file_name2, "w");
    FILE *fp3 = fopen("./check//text3.txt", "w");
    */
    return EXIT_SUCCESS;
}
struct movies *createMovie(char *current_Line)
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
struct movies *processFile(char *filePath)
{
    /* only read */
    FILE *movieFile = fopen(filePath, "r");
    int number_of_movies=0;
    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    /* The head and tail of the linked list */
    struct movies *head = NULL;
    struct movies *tail = NULL;
    /* do need to store info of the first line */
    getline(&currLine, &len, movieFile);
    /* Read the file line by line */
    while ((nread = getline(&currLine, &len, movieFile)) != -1){
        /* create a new movies node for each lines */
        struct movies *newNode = createMovie(currLine);
        ++number_of_movies;
        /* if linked list is empty, head = tail = newNode */
        if (head == NULL){
            head = newNode;
            tail = newNode;
        }else{
            /*
                updata current tail's next to newNode and tail to newNode
                Note: newNode's tail is NULL
            */
            tail->next = newNode;
            tail = newNode;
        }
    }
    printf("Processed file %s and parsed data for %d movies\n", filePath, number_of_movies);
    /* should free the memory that is never used again */
    free(currLine);
    /*close file*/
    fclose(movieFile);

    return head;
}
