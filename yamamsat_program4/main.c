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

void *input_thread();
void *line_separator_thread();
void *plus_sign_thread();
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

void *input_thread()
{
    int quit = 0;
    char input[SIZE];
    for(int i = 0; i < NUM_ITEMS && !quit; ++i){
        /* reset imput buffer */
        memset(input, 0, SIZE);
        /* reads in lines of charactors from the standard input */
        read(0, input, SIZE-1);

        /* lock the mutex before putting the item in the buffer */
        pthread_mutex_lock(&mutex_1);
        /*copy the */
        strcpy(buffer_1[prod_idx_1],input);

        /* Increment the index where the next item will be put.*/
        prod_idx_1 = prod_idx_1 + 1;
        /* count　unprocessed input */
        count_1++;

        if(strncmp(input, "quit\n", 5) == 0){
            quit = 1;
        }
        /* Signal to the consumer that the buffer is no longer empty */
        pthread_cond_signal(&full_1);
        /* Unlock the mutex */
        pthread_mutex_unlock(&mutex_1);
    }
    return NULL;
}
void *line_separator_thread()
{
    return NULL;
}
void *plus_sign_thread()
{
    return NULL;
}
void *output_thread()
{
    int quit = 0;
    for(int i = 0; i < NUM_ITEMS && !quit; ++i){

        pthread_mutex_lock(&mutex_1);

        while(count_1 == 0){
            pthread_cond_wait(&full_1, &mutex_1);
        }



        if(strncmp(input, "quit\n", 5) == 0){
            quit = 1;
        }else{
            printf("test: %s\n", buffer_1[con_idx_1]);
            ++con_idx_1;
            --count_1;
        }
        pthread_mutex_unlock(&mutex_1);
    }
    return NULL;
}