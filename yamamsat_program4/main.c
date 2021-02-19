/*
    ONID: yamamsat
    email: yamamsat@oregonstate.edu

    NOTE: I avoid to use single-line comment because this was created in cpp
    If I remember correctly, some of old compiler display error message for this.

    Here is the links where I checked while I was making Programming memo

    reference:

    an example program from cs344

    - for some information
    cs344 Explorations

**********************************************************************************************/
/*  TODO
    Thread 1, called the Input Thread, reads in lines of characters from the standard input.

    Thread 2, called the Line Separator Thread, replaces every line separator in the input by a space.
    '\n' to space
    Thread 3, called the Plus Sign thread, replaces every pair of plus signs, i.e., "++", by a "^".
    replace ++ to ^
    --count
    Thread 4, called the Output Thread, write this processed data to standard output as lines of exactly 80 characters.
    count the charactors and display
**********************************************************************************************/
/*
    pickup info
    The input will not contain any empty lines, i.e., lines that only have space characters or no characters except the line separator.
    An input line will never be longer than 1000 characters (including the line separator).
    The input for the program will never have more than 49 lines before the stop-processing line.
    Your program doesn't need to check the input for correctness..
************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/* size of input buffers */
#define SIZE 1000
/* number of input lines (I may use 50 to read last input) */
#define NUM_ITEMS 49

/* number of output charactors per line */
#define NUM_DISPLAY 80

/* This buffer is used by Input thread and Line separator thread */
char buffer_1[NUM_ITEMS][SIZE];
/* Number of items in the buffer 1 */
int count_1 = 0;
/* Index where the input thread will put the next item */
int prod_idx_1 = 0;
/* Index where the line separator thread will pick up the next item */
int con_idx_1 = 0;
/* Initialize the mutex for buffer 1 */
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
/* Initialize the condition variable for buffer 1 */
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;

/* This buffer is used by Line separator thread and Plus sign thread */
char buffer_2[NUM_ITEMS][SIZE];
/* Number of items in the buffer 2 */
int count_2 = 0;
/* Index where the input thread will put the next item */
int prod_idx_2 = 0;
/* Index where the plus sign thread will pick up the next item */
int con_idx_2 = 0;
/* Initialize the mutex for buffer 2 */
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
/* Initialize the condition variable for buffer 2 */
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;

/* This buffer is used by Plus sign thread and Output thread */
char buffer_3[NUM_ITEMS][SIZE];
/* Number of items in the buffer 3 */
int count_3 = 0;
/* Index where the input thread will put the next item */
int prod_idx_3 = 0;
/* Index where the output thread will pick up the next item */
int con_idx_3 = 0;
/* Initialize the mutex for buffer 3 */
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
/* Initialize the condition variable for buffer 3 */
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

char buffer_4[SIZE];

void *input_thread();
void get_buffer_1(char *str);

void *line_separator_thread();
void put_buff_2(char *str);
void get_buff_2(char *str);

void *plus_sign_thread();
void put_buff_3(char *str);
void get_buff_3(char *str);

void *output_thread();

int main()
{
    pthread_t input_t, line_separator_t, plus_sign_t, output_t;

    pthread_create(&input_t, NULL, input_thread, NULL);
    pthread_create(&line_separator_t, NULL, line_separator_thread, NULL);
    pthread_create(&plus_sign_t, NULL, plus_sign_thread, NULL);
    pthread_create(&output_t, NULL, output_thread, NULL);


    // Wait for the threads to terminate
    pthread_join(input_t, NULL);
    pthread_join(line_separator_t, NULL);
    pthread_join(plus_sign_t, NULL);
    pthread_join(output_t, NULL);

    return EXIT_SUCCESS;
}
/*
    reads in lines of charactors from the standard input and store data in buffer_1
    while it stores data, function (ine_separator thread) that uses buffer_1 is locked.
    after that, this send a sigal to line_separator thread
    once this detect the input of "STOP", stop reading data.
***************************************************/
void *input_thread()
{
    int quit = 0;
    char input[SIZE];
    for(int i = 0; i < NUM_ITEMS && !quit; ++i){
        /* reset imput buffer */
        memset(input, 0, SIZE);
        /* reads in lines of charactors from the standard input */
        fgets(input, SIZE-1, stdin);
        /* lock the mutex before putting the item in the buffer */
        pthread_mutex_lock(&mutex_1);
        /*copy the */
        strcpy(buffer_1[prod_idx_1],input);

        /* Increment the index where the next item will be put.*/
        prod_idx_1 = prod_idx_1 + 1;
        /* count　unprocessed input */
        count_1++;

        if(strncmp(input, "STOP\n", 5) == 0){
            quit = 1;
        }
        /* Signal to the consumer that the buffer is no longer empty */
        pthread_cond_signal(&full_1);
        /* Unlock the mutex */
        pthread_mutex_unlock(&mutex_1);
    }
    return NULL;
}
void get_buffer_1(char *str)
{
    /* Lock the mutex before checking if the buffer has data */
    pthread_mutex_lock(&mutex_1);
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    while (count_1 == 0){
        pthread_cond_wait(&full_1, &mutex_1);
    }
    strcpy(str, buffer_1[con_idx_1]);
    // Increment the index from which the item will be picked up
    con_idx_1 = con_idx_1 + 1;
    count_1--;
    // Unlock the mutex
    pthread_mutex_unlock(&mutex_1);
}
/*
************************************************************************************/
void *line_separator_thread()
{
    int quit = 0;
    /* This input store string from buffer_1 */
    char input[SIZE];
    for(int i = 0; i < NUM_ITEMS && !quit; ++i){
        /* reset imput buffer */
        memset(input, 0, SIZE);
        get_buffer_1(input);
        /*printf("test2: %s", input);
        fflush(stdout);*/
        if(strncmp(input, "STOP\n", 5) == 0){
            quit = 1;
        }else{
            char *tmp = strchr(input, '\n');
            if(tmp != NULL){
                *tmp = ' ';
            }
        }
        put_buff_2(input);
    }

    return NULL;
}
void put_buff_2(char *str)
{
    // Lock the mutex before putting the item in the buffer
    pthread_mutex_lock(&mutex_2);
    // Put the item in the buffer
    strcpy(buffer_2[prod_idx_2], str);
    // Increment the index where the next item will be put.
    ++prod_idx_2;
    ++count_2;
    // Signal to the consumer that the buffer is no longer empty
    pthread_cond_signal(&full_2);
    // Unlock the mutex
    pthread_mutex_unlock(&mutex_2);
}
void get_buff_2(char *str)
{
    pthread_mutex_lock(&mutex_2);

    while(count_2 == 0){
        pthread_cond_wait(&full_2, &mutex_2);
    }
    strcpy(str, buffer_2[con_idx_2]);
    con_idx_2 = con_idx_2 + 1;
    count_2--;

    pthread_mutex_unlock(&mutex_2);
}

void *plus_sign_thread()
{
    int quit = 0;
    /* This input store string from buffer_2 */
    char input[SIZE];
    for(int i = 0; i < NUM_ITEMS && !quit; ++i){
        /* reset imput buffer */
        memset(input, 0, SIZE);
        get_buff_2(input);
        /*printf("test2: %s", input);
        fflush(stdout);*/
        if(strncmp(input, "STOP\n", 5) == 0){
            quit = 1;
        }else{

            /* it can replace string of " " to to_this*/
            char *to_this = "^";
            char *tmp = input;

            while((tmp = strstr(tmp, "++")) != NULL){
                /* tmp2 is pointing after "++" */
                const char *tmp2 = tmp + 2;
                /* move tmp2 after tmp + strlen(to_this) */
                memmove(tmp + strlen(to_this), tmp2, strlen(tmp2)+1);
                /* copy to_this to tmp */
                memcpy(tmp, to_this, strlen(to_this));
                /* next pointer */
                tmp+=strlen(to_this);
            }
        }
        put_buff_3(input);
    }

    return NULL;
}

void put_buff_3(char *str)
{
    // Lock the mutex before putting the item in the buffer
    pthread_mutex_lock(&mutex_3);
    // Put the item in the buffer
    strcpy(buffer_3[prod_idx_3], str);
    // Increment the index where the next item will be put.
    ++prod_idx_3;
    ++count_3;
    // Signal to the consumer that the buffer is no longer empty
    pthread_cond_signal(&full_3);
    // Unlock the mutex
    pthread_mutex_unlock(&mutex_3);
}
void get_buff_3(char *str)
{
    pthread_mutex_lock(&mutex_3);

    while(count_3 == 0){
        pthread_cond_wait(&full_3, &mutex_3);
    }
    strcpy(str, buffer_3[con_idx_3]);
    con_idx_3 = con_idx_3 + 1;
    count_3--;

    pthread_mutex_unlock(&mutex_3);
}
/*

*************************************************************/
void *output_thread()
{

    int quit = 0;
    char input[SIZE];
    for(int i = 0; i < NUM_ITEMS && !quit; ++i){
        memset(input, 0, SIZE);
        get_buff_3(input);

        if(strncmp(input, "STOP\n", 5) == 0){
            quit = 1;
        }else{
            printf("%s\n", input);
            /* we need to display from stdout stream imadiately */
            fflush(stdout);
        }
    }

    return NULL;
}