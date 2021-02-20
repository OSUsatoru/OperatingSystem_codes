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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/*  size of input buffers
    An input line will never be longer than 1000 characters (including the line separator).*/
#define SIZE 1000
/*  number of input lines (I may use 50 to read last input)
    The input for the program will never have more than 49 lines before the stop-processing line.*/
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

/* This buffer is used by only Output thread, so I do not need to lock this area */
char buffer_4[SIZE]="";


void *input_thread();
void get_buffer_1(char *str);

void *line_separator_thread();
void put_buff_2(char *str);
void get_buff_2(char *str);

void *plus_sign_thread();
void put_buff_3(char *str);
void get_buff_3(char *str);

void *output_thread();

/*
    The main function created threads and wait for the threads to terminate
**************************************************************/
int main()
{
    pthread_t input_t, line_separator_t, plus_sign_t, output_t;

    /* create threads */
    pthread_create(&input_t, NULL, input_thread, NULL);
    pthread_create(&line_separator_t, NULL, line_separator_thread, NULL);
    pthread_create(&plus_sign_t, NULL, plus_sign_thread, NULL);
    pthread_create(&output_t, NULL, output_thread, NULL);


    /* Wait for the threads */
    pthread_join(input_t, NULL);
    pthread_join(line_separator_t, NULL);
    pthread_join(plus_sign_t, NULL);
    pthread_join(output_t, NULL);

    return EXIT_SUCCESS;
}
/*
    This reads in lines of charactors from the standard input and store data in buffer_1
    while it stores data, function that uses buffer_1 is locked.
    after that, this send a sigal to line_separator thread
    once this detect the input of "STOP", stop reading data.
    The input will not contain any empty lines, i.e., lines that only have space characters or no characters except the line separator.
    Your program doesn't need to check the input for correctness.
***************************************************/
void *input_thread()
{
    int quit = 0;
    char input[SIZE];
    for(int i = 0; i < NUM_ITEMS && !quit; ++i){
        /* reset input buffer */
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

        /* if input is "STOP"*/
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
/*
    This function wait the signal of the producer and copy the data from buffer_1 to str
    function that uses buffer_1 is locked until this process is done.
***********************************************************************/
void get_buffer_1(char *str)
{
    /* Lock the mutex before checking if the buffer has data */
    pthread_mutex_lock(&mutex_1);
    /* Buffer is empty. Wait for the producer to signal that the buffer has data */
    while (count_1 == 0){
        pthread_cond_wait(&full_1, &mutex_1);
    }
    /* copy input strnig to str */
    strcpy(str, buffer_1[con_idx_1]);
    /* Increment the index from which the item will be picked up */
    con_idx_1 = con_idx_1 + 1;
    /* decrease counter */
    count_1--;
    /* Unlock the mutex */
    pthread_mutex_unlock(&mutex_1);
}
/*
    This function gets data from buffer_1 and replce '\n' to SPACE.
    Then put data to buffer_2
    once this detect the input of "STOP", stop reading data.
************************************************************************************/
void *line_separator_thread()
{
    int quit = 0;
    /* This input store string from buffer_1 */
    char input[SIZE];
    for(int i = 0; i < NUM_ITEMS && !quit; ++i){
        /* reset input buffer */
        memset(input, 0, SIZE);
        /* store data from buffer_1 to input */
        get_buffer_1(input);

        /*printf("test2: %s", input);
        fflush(stdout);*/

        /* If input is not "STOP", replce '\n' to SPACE */
        if(strncmp(input, "STOP\n", 5) == 0){
            quit = 1;
        }else{
            char *tmp = strchr(input, '\n');
            if(tmp != NULL){
                *tmp = ' ';
            }
        }
        /* store edited data to buffer_2 */
        put_buff_2(input);
    }

    return NULL;
}
/*
    This store received data to buffer_2
    the function that uses buffer_2 is locked until this process is done.
******************************************************************************/
void put_buff_2(char *str)
{
    /* Lock the mutex before putting the item in the buffer */
    pthread_mutex_lock(&mutex_2);
    /* Put the item in the buffer */
    strcpy(buffer_2[prod_idx_2], str);
    /* Increment the index where the next item will be put. */
    ++prod_idx_2;
    /* Increment the counter */
    ++count_2;
    /* Signal to the consumer that the buffer is no longer empty */
    pthread_cond_signal(&full_2);
    /* Unlock the mutex */
    pthread_mutex_unlock(&mutex_2);
}
/*
    This function wait the signal of the producer and copy the data from buffer_2 to str
    function that uses buffer_2 is locked until this process is done.
***********************************************************************/
void get_buff_2(char *str)
{
    /* Lock the mutex before checking if the buffer has data */
    pthread_mutex_lock(&mutex_2);
    /* Buffer is empty. Wait for the producer to signal that the buffer has data */
    while(count_2 == 0){
        pthread_cond_wait(&full_2, &mutex_2);
    }
    /* copy input strnig to str */
    strcpy(str, buffer_2[con_idx_2]);
    /* Increment the index from which the item will be picked up */
    con_idx_2 = con_idx_2 + 1;
    /* decrease counter */
    count_2--;
    /* Unlock the mutex */
    pthread_mutex_unlock(&mutex_2);
}
/*
    This function gets data from buffer_2 and replce '++' to '^'.
    Then put data to buffer_3
    once this detect the input of "STOP", stop reading data.
************************************************************************************/
void *plus_sign_thread()
{
    int quit = 0;
    /* This input store string from buffer_2 */
    char input[SIZE];
    for(int i = 0; i < NUM_ITEMS && !quit; ++i){
        /* reset input buffer */
        memset(input, 0, SIZE);
        /* store data from buffer_2 to input */
        get_buff_2(input);

        /*printf("test2: %s", input);
        fflush(stdout);*/

        /* if input is not "STOP", replce '++' to '^' */
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
        /* store edited data to buffer_3 */
        put_buff_3(input);
    }

    return NULL;
}
/*
    This store received data to buffer_3
    the function that uses buffer_3 is locked until this process is done.
******************************************************************************/
void put_buff_3(char *str)
{
    /* Lock the mutex before putting the item in the buffer */
    pthread_mutex_lock(&mutex_3);
    /* Put the item in the buffer */
    strcpy(buffer_3[prod_idx_3], str);
    /* Increment the index where the next item will be put. */
    ++prod_idx_3;
    /* Increment the counter */
    ++count_3;
    /* Signal to the consumer that the buffer is no longer empty */
    pthread_cond_signal(&full_3);
    /* Unlock the mutex */
    pthread_mutex_unlock(&mutex_3);
}
/*
    This function wait the signal of the producer and copy the data from buffer_3 to str
    function that uses buffer_3 is locked until this process is done.
***********************************************************************/
void get_buff_3(char *str)
{
    /* Lock the mutex before checking if the buffer has data */
    pthread_mutex_lock(&mutex_3);
    /* Buffer is empty. Wait for the producer to signal that the buffer has data */
    while(count_3 == 0){
        pthread_cond_wait(&full_3, &mutex_3);
    }
    /* copy input strnig to str */
    strcpy(str, buffer_3[con_idx_3]);
    /* Increment the index from which the item will be picked up */
    con_idx_3 = con_idx_3 + 1;
    /* decrease counter */
    count_3--;
    /* Unlock the mutex */
    pthread_mutex_unlock(&mutex_3);
}
/*
    This function gets data from buffer_3 and dispaly 80 charactors.

    once this detect the input of "STOP", stop reading data.
************************************************************************************/
void *output_thread()
{

    int quit = 0;

    char input[SIZE];


    for(int i = 0; i < NUM_ITEMS && !quit; ++i){
        /* reset input buffer */
        memset(input, 0, SIZE);
        /* store data from buffer_3 to input */
        get_buff_3(input);

        /*  if input is not "STOP", store 80 charactors to buffer_4 and display if there
            are enough charactors */
        if(strncmp(input, "STOP\n", 5) == 0){
            quit = 1;
        }else{
            char *p_input = input;
            int input_size = strlen(p_input), buff_size = strlen(buffer_4);
            /*
            printf("input:%d\n", input_size);
            fflush(stdout);
            printf("buffer:%d\n", buff_size);
            fflush(stdout);
            */
            /* if there are enough charactors to display */
            while(input_size + buff_size >= NUM_DISPLAY ){
                /* need (NUM_DISPLAY - buff_size) charactors*/
                int need_chars = NUM_DISPLAY - buff_size;
                /* combine buffer_4 and input */
                strncat(buffer_4, p_input, need_chars);

                /* display */
                printf("%s\n", buffer_4);
                /* we need to display from stdout stream imadiately */
                fflush(stdout);
                /* clearn up buffer_4 */
                memset(buffer_4, 0, SIZE);
                /* move to next charactor */
                p_input+=need_chars;

                input_size = strlen(p_input);
                buff_size = strlen(buffer_4);
            }
            /* store rest of data to buffer_4 */
            strcat(buffer_4, p_input);

        }
    }

    return NULL;
}