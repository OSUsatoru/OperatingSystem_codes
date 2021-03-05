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
int initial_contact();
void send_msg();
void receive_msg();
/*
    argv[1]: plain text
    argv[2]: key test
    argv[3]: port numer
**************************************************/
int main(int argc, char *argv[]) {

    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    char buffer[256];

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
    /*
    // Get input message from user
    printf("CLIENT: Enter text to send to the server, and then hit enter: ");
    // Clear out the buffer array
    memset(buffer, '\0', sizeof(buffer));
    // Get input from the user, trunc to buffer - 1 chars, leaving \0
    fgets(buffer, sizeof(buffer) - 1, stdin);
    // Remove the trailing \n that fgets adds
    buffer[strcspn(buffer, "\n")] = '\0';
    */

    // Send message to server
    // Write to the server
    fprintf(stdout,"CLIENT: size of file: %d\n", key_size);
    fflush(stdout);
    charsWritten = send(socketFD, key_buffer, key_size+1, 0);
    if (charsWritten < 0){
        fprintf(stderr,"CLIENT: ERROR writing to socket");
    }
    if (charsWritten < key_size+1){
        fprintf(stderr,"CLIENT: WARNING: Not all data written to socket!\n");
    }

    // Get return message from server
    // Clear out the buffer again for reuse
    //memset(buffer, '\0', sizeof(buffer));
    // Read data from the socket, leaving \0 at end

    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
    if (charsRead < 0){
        fprintf(stderr,"CLIENT: ERROR reading from socket");
    }
    fprintf(stdout,"CLIENT: I received this from the server: \"%s\"\n", buffer);

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

    //printf("%d\n%d\n", text_size, key_size);


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
    text_size = strlen(text_buffer)-1;
    key_size = strlen(key_buffer)-1;

    if(text_size > key_size){
        fprintf(stderr,"Error: key '%s' is too short\n", key);
        exit(1);
    }

    /* Replace '\n' to NULL character */
    text_buffer[strlen(text_buffer)] = '\0';
    key_buffer[strlen(key_buffer)] = '\0';
    //printf("%s:%d\n", text_buffer, text_size);
    //printf("%s:%d\n", key_buffer, key_size);

    /*
        This is not efficient though
    ************************************************/
    for(int i = 0; i < text_size; ++i){
        /* If text has a bad character */
        if(strchr(key_characters, text_buffer[i]) == NULL){
            fprintf(stderr,"Error: '%s' has a bad character\n", text);
            exit(1);
        }
    }
    for(int i = 0; i < key_size; ++i){
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
int initial_contact();
/*
    This will send the string information to server.
    first: send the size of data
    second: send the string data
**********************************************************/
void send_msg();
/*
    This will receive the string data from server
    first: receive the size of data
    second: receive the string data
********************************************************/
void receive_msg();