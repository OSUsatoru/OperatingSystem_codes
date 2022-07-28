/*
    ONID: yamamsat
    email: yamamsat@oregonstate.edu

    NOTE: I avoid to use single-line comment because this was created in cpp
    If I remember correctly, some of old compiler display error message for this.

    Here is the links where I checked while I was making Programming memo

    reference:
    - for setprecisioni in c
    https://www.k-cube.co.jp/wakaba/server/format.html
    - for strtod
    http://www.c-tipsref.com/reference/stdlib/strtod.html
    - for malloc and calloc
    https://c.keicode.com/lib/stdlib-memory.php
    - for getline
    https://linuxjm.osdn.jp/html/LDP_man-pages/man3/getline.3.html
    - for strtok_r
    http://manpages.ubuntu.com/manpages/trusty/ja/man3/strtok.3.html
    - for strncpy
    http://www.cplusplus.com/reference/cstring/strncpy/
**********************************************************************************************/
/*
    Programming memo:

    malloc(value);
    calloc(value, sizeof(char));
    -------------------------------
    ssize_t getline(char **lineptr, size_t *n, FILE *stream);
    linerptr stores the address of input string
    return -1 for error or end of file
    -----------------------------
    use atoi instead of using stoi
    strtod instead of using stod

    strtod( const char* nptr, char **restrict endptr); Return: result value for valid input, otherwise 0;
    endptr stores characters after numerical value. this can be NULL
    ------------------------------
    split string
    char *strtok_r(char *str, const char *delim, char **saveptr);
    -----------------------------------
    - dynamic allocating array (I did not use in my code)
    http://rainbow.pc.uec.ac.jp/edu/program/b1/Prob6-2.htm

******************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

struct movies *createMovie(char *current_Line);
struct movies *processFile(char *filePath);
void display_movies_filtered_by_year(struct movies *list);
void display_highest_rate_movies(struct movies *list);
void display_movies_filtered_by_language(struct movies *list);
/* for debugging */
void test_printMovies(const struct movies *movie);
void test_printMoviesList(struct movies *list);
/*
    title: string
    year: int
    languages: store all language in string (e.g) if input: [English;Russian], language[] = "english;russian";
    rating: double ( can be float )
    next: addres of next movie
*********************************************************************/
struct movies{
    char *title;
    int year;
    char *languages;
    double rating;
    struct movies *next;
};
/*
    argc: number of command line arguments
    argv: argv[0] stores the name of file with path
          and following arguments stores in each element

    gcc --std=gnu99 -o movie main.c
    ./movie.exe movies_sample_1.csv

    In this case, argc = 2,
    argv[0] = file path of movies.exe
    argv[1] = movies_sample_1.csv
*****************************************/
int main(int argc, char *argv[])
{
    if (argc < 2){
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movies_sample_1.csv\n");
        return EXIT_FAILURE;
    }
    int choice;
    struct movies *movie_list = processFile(argv[1]);

    do{
        printf("\n1. Show movies released in the specified year\n");
        printf("2. Show highest rated movie for each year\n");
        printf("3. Show the title and year of release of all movies in a specific language\n");
        printf("4. Exit from the program\n\n");
        printf("Enter a choice from 1 to 4: ");
        scanf("%d", &choice);

        switch(choice){
            case 1:
                display_movies_filtered_by_year(movie_list);
                break;
            case 2:
                display_highest_rate_movies(movie_list);
                break;
            case 3:
                display_movies_filtered_by_language(movie_list);
                break;
            case 4:
                break;
            default:
                printf("You entered an incorrect choice. Try again.\n");
                break;
        }
    }while(choice!=4);

    return EXIT_SUCCESS;
}
/*
    this receives the string of one line.
    once create the new movie information, this->next = NULL and return the address of this
    NOTE: every inputs are separeted by ',' and follows the data type of sturuct movies
************************************************************************/
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
/*
    this receive the filePath and open file
    by using previous function to create new movies node and updata linked list info
    at the same time
    display filePath and number of inputs
**********************************************************************************************/
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
/*
    For user input 1
    Get user input and display the names of all the movies released in that year
    if the data does not have any movies in that year, print a message about this
**********************************************************************************/
void display_movies_filtered_by_year(struct movies *list)
{
    int u_year, cnt = 0;
    printf("Enter the year for which you want to see movies: ");
    scanf("%d", &u_year);
    /*because values of year will be between 1900 and 2021 (inclusive of these years)*/
    if(u_year < 1900 || u_year > 2021){
        printf("No data about movies released in the year %d\n", u_year);
        return;
    }
    while(list!=NULL){
        if(list->year == u_year){
            cnt++;
            printf("%s\n", list->title);
        }
        list = list->next;
    }
    /* if there is no movie for user input*/
    if(!cnt){
        printf("No data about movies released in the year %d\n", u_year);
    }
}
/*
    display a movie that had the highest rating along with the year
****************************************************************/
void display_highest_rate_movies(struct movies *list)
{
    /*
        because values of year will be between 1900 and 2021 (inclusive of these years)
        we only need 122 elements to store (2021 - 1900 + 1)
        this years stores the index of where the movie info is stored
        its index is corresponding to [value of year - 1900]
        if years[index] == -1, highest_rate_movies.push_back(movie);
        else compare highest_rate_movies[years[index]]->rating and rating
    */
    /* index for highest_rate_movies */
    int cnt=0;
    int years[122];
    struct movies *highest_rate_movies[122];
    /* initialize array elements */
    memset(years, -1, sizeof(years));
    memset(highest_rate_movies, '\0', sizeof(highest_rate_movies));

    while(list!=NULL){
        /* highest_rate_movies does not hold a movie that has same year*/
        if(years[list->year-1900] == -1){
            years[list->year-1900] = cnt;
            highest_rate_movies[cnt] = list;
            ++cnt;
        }else{
            /*highest_rate_movies holds a movie that has same year */
            /*compare rate*/
            if(highest_rate_movies[years[list->year-1900]] -> rating < list->rating ){
                highest_rate_movies[years[list->year-1900]] = list;
            }
        }
        list = list->next;
    }
    /*display stored movies*/
    for(int i = 0; i < cnt; ++i){
        printf("%d %.1lf %s\n", highest_rate_movies[i]->year,
                                highest_rate_movies[i]->rating,
                                highest_rate_movies[i]->title);
    }
}
/*
    Get user input and display the names of all the movies released in that language
    You should only do an exact match on the language entered by the user
    E.g., "English" shouldn’t match "english"
************************************************************************************/
void display_movies_filtered_by_language(struct movies *list)
{
    /*ou can assume that the length of the language string entered by the user will be less than 20 character.*/
    char u_language[25], *lang, *saveptr;
    int cnt = 0;
    printf("Enter the language for which you want to see movies: ");
    scanf("%s", u_language);

    while(list!=NULL){
        char *lang = strtok_r(list->languages, ";", &saveptr);
        while(lang){
            if(!strcmp(lang, u_language)){
                printf("%d %s\n", list->year, list->title);
                cnt++;
                break;
            }

            lang = strtok_r(NULL, ";", &saveptr);
        }
        list = list->next;
    }
    if(!cnt){
        printf("No data about movies released in %s\n", u_language);
    }
}
/*
    this is a test functions for input and output file information
**************************************************************/
void test_printMovies(const struct movies *movie)
{
    printf("%s %d %s %.1lf\n", movie->title,
               movie->year,
               movie->languages,
               movie->rating);
}
void test_printMoviesList(struct movies *list)
{
    while (list != NULL){
        test_printMovies(list);
        list = list->next;
    }
}