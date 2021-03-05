/*
    ONID: yamamsat
    email: yamamsat@oregonstate.edu

    NOTE: I avoid to use single-line comment because this was created in cpp
    If I remember correctly, some of old compiler display error message for this.

    Here is the links where I checked while I was making Programming memo

    reference:

**********************************************************************************************/
/*
    1. send "enc_client". this will receive the message from server
    2. send the size of file to server.
    3. send the data to server.
    4. receive the message from server
***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

char key_characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

char *text_buffer;
char *key_buffer;

/* Eventually this size does not include NULL character */
unsigned long text_size, key_size;

void Is_Valid_Files(const char *text, const char *key);
void setupAddressStruct(struct sockaddr_in* address, int portNumber);
int initial_contact(int socketFD);
void send_text_msg(int socketFD);
void send_key_msg(int socketFD);
void receive_msg(int socketFD);
/*
    argv[1]: plain text
    argv[2]: key test
    argv[3]: port numer
**************************************************/
int main(int argc, char *argv[]) {

    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;



    /* if there is no enogut arguments */
    if (argc < 4) {
        fprintf(stderr,"USAGE: %s plaintext keytext port\n", argv[0]);
        exit(1);
    }
    /* Check if files are valid */
    Is_Valid_Files(argv[1], argv[2]);

    fprintf(stdout,"CLIENT: checked file\n");
    fflush(stdout);

    /* Create a socket: IPv4, TCP */
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    /* If faild to createe the socket */
    if (socketFD < 0){
        fprintf(stderr,"CLIENT: ERROR opening socket\n");
        exit(1);
    }

    /* Set up the server address struct */
    setupAddressStruct(&serverAddress, atoi(argv[3]));

    /* Try to connect  */
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
        fprintf(stderr,"CLIENT: ERROR connecting");
        exit(1);
    }
    /* First contact to see if it connects correct server */

    if(initial_contact(socketFD) == 0){
        fprintf(stderr,"CLIENT: ERROR Connection is rejected by server.");
        exit(2);
    }
    fprintf(stdout, "%s\n", key_buffer );
    fprintf(stdout, "%s\n", text_buffer);

    fprintf(stdout, "%d\n", key_size );
    fprintf(stdout, "%d\n", text_size);
    fflush(stdout);

    /* send text_buffer */
    send_text_msg(socketFD);
    /* send key_buffer */
    //send_key_msg(socketFD);

    /* receive message form server */
    receive_msg(socketFD);

    // Close the socket
    close(socketFD);
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
    text_size = text_Stat.st_size;
    key_size = key_Stat.st_size;

    //printf("%d\n%d\n", *text_size, key_size);


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
    text_buffer = malloc(text_size);
    key_buffer = malloc(key_size);
    /* Store string including '\n' */
    fgets(text_buffer, text_size, text_file);
    fgets(key_buffer, key_size, key_file);
    /* Close the files */
    fclose(text_file);
    fclose(key_file);
    /* Updata size of file */
    text_size = strlen(text_buffer);
    key_size = strlen(key_buffer);

    if(text_size > key_size){
        fprintf(stderr,"Error: key '%s' is too short\n", key);
        exit(1);
    }

    /* Replace '\n' to NULL character
    text_buffer[text_size] = '\0';
    key_buffer[key_size] = '\0';*/
    //printf("%s:%d\n", text_buffer, text_size);
    //printf("%s:%d\n", key_buffer, key_size);

    /*
        This is not efficient though
    ************************************************/
    for(int i = 0; i < text_size-1; ++i){
        /* If text has a bad character */
        if(strchr(key_characters, text_buffer[i]) == NULL){
            fprintf(stderr,"Error: '%s' has a bad character\n", text);
            exit(1);
        }
    }
    for(int i = 0; i < key_size-1; ++i){
        /* If key has a bad character */
        if(strchr(key_characters, key_buffer[i]) == NULL){
            fprintf(stderr,"Error: '%s' has a bad character\n", key);
            exit(1);
        }
    }

}

/* Set up the address struct
********************************************************************************/
void setupAddressStruct(struct sockaddr_in* address, int portNumber){
    char IPaddress[] = "127.0.0.1";
    // Clear out the address struct
    memset((char*) address, '\0', sizeof(*address));

    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);

    // Get the DNS entry for this host name
    struct hostent* hostInfo = gethostbyname(IPaddress);
    if (hostInfo == NULL) {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
    }
    // Copy the first IP address from the DNS entry to sin_addr.s_addr
    memcpy((char*) &address->sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);

}
/*
    This will send program name to server and receive the response
    from server
    return the received value.
    1 for valid
    0 for invalid connect
**********************************************************/
int initial_contact(int socketFD)
{
    /* buffer will reseive the character and '\0' from server */
    int respons;
    char name[] = "enc_client\0";
    if(send(socketFD, name, strlen(name), 0) < 0){
        fprintf(stderr, "Client: Error Faild to send an initial message to server\n");
        exit(2);
    }

    if(recv(socketFD, &respons, sizeof(respons), 0) < 0){
        fprintf(stderr, "Client: Errir Faild to receive an initial message from server\n");
        exit(2);
    }
    fprintf(stdout, "test on client: %d\n", respons);
    fflush(stdout);

    return respons;
}
/*
    This will send the string information to server.
    first: send the size of data
    second: send the string data
**********************************************************/
void send_text_msg(int socketFD)
{
    if(send(socketFD, &text_size, sizeof(text_size), 0) < 0){
        fprintf(stderr, "Client: Error Faild to send an initial message to server\n");
        exit(2);
    }
    fprintf(stdout, "stelen: %d\n", strlen(text_buffer));
    fflush(stdout);

    if(send(socketFD, text_buffer, text_size, 0) < 0){
        fprintf(stderr, "Client: Error Faild to send an initial message to server\n");
        exit(2);
    }

}
void send_key_msg(int socketFD)
{


}
/*
    This will receive the string data from server
    first: receive the size of data
    second: receive the string data
********************************************************/
void receive_msg(int socketFD)
{

}