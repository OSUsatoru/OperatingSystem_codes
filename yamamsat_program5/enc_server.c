/*
    ONID: yamamsat
    email: yamamsat@oregonstate.edu

    NOTE: I avoid to use single-line comment because this was created in cpp
    If I remember correctly, some of old compiler display error message for this.

    Here is the links where I checked while I was making Programming memo

    reference:

**********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void setupAddressStruct(struct sockaddr_in* address, int portNumber);
void childProcess(int connectionSocket);
int initial_contact(int connectionSocket);
void send_msg(int connectionSocket);
void receive_text_msg(int connectionSocket);
void receive_key_msg(int connectionSocket);

int main(int argc, char *argv[]){
    int connectionSocket, charsRead;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo = sizeof(clientAddress);

    // Check usage & args
    if (argc < 2) {
        fprintf(stderr,"USAGE: %s port\n", argv[0]);
        exit(1);
    }

    // Create the socket that will listen for connections
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        fprintf(stderr, "ERROR opening socket");
        exit(1);
    }

    // Set up the address struct for the server socket
    setupAddressStruct(&serverAddress, atoi(argv[1]));

    // Associate the socket to the port
    if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
        fprintf(stderr, "ERROR on binding");
        exit(1);
    }

    // Start listening for connetions. Allow up to 5 connections to queue up
    listen(listenSocket, 5);


    // Accept a connection, blocking if one is not available until one connects
    while(1){
        // Accept the connection request which creates a connection socket
        connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
        if (connectionSocket < 0){
            fprintf(stderr, "ERROR on accept");
            exit(1);
        }

        printf("SERVER: Connected to client running at host %d port %d\n",ntohs(clientAddress.sin_addr.s_addr), ntohs(clientAddress.sin_port));
        pid_t spawnPid;

        spawnPid = fork();

        switch(spawnPid){
            case -1:
                fprintf(stderr, "Server: Error fork() failed\n");
                exit(1);
                break;
            case 0: /* This is on child process */
                childProcess(connectionSocket);
                close(connectionSocket);
                exit(0);
                break;
            default:
                break;
        }

        close(connectionSocket);
    }
    // Close the listening socket
    close(listenSocket);
    return 0;
}


// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, int portNumber){
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address));

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}
/*
    This will receive program name from client and send the response
    to client
    1 for valid
    0 for invalid connect
**********************************************************/
void childProcess(int connectionSocket)
{
    if(initial_contact(connectionSocket) == 0){
        close(connectionSocket);
        exit(2);
    }

    receive_text_msg(connectionSocket);
    receive_key_msg(connectionSocket);
    // Close the connection socket for this client
}
int initial_contact(int connectionSocket)
{
    char recv_name[256];
    int reply;
    memset(recv_name, '\0', 256);

    if(recv(connectionSocket, recv_name, 255, 0) < 0){
        fprintf(stderr, "Server: Error Faild to receive an initial message from client\n");
        exit(1);
    }
    fprintf(stdout, "test on server: %s\n", recv_name);
    fflush(stdout);
    if(strcmp(recv_name, "enc_client\0") == 0){
        reply = 1;
    }else{
        reply = 0;
    }
    fprintf(stdout, "test on server: %d\n", reply);
    fflush(stdout);

    if(send(connectionSocket, &reply, sizeof(reply), 0) < 0){
        fprintf(stderr, "Server: Error Faild to send an initial message to server\n");
        exit(1);
    }
    return reply;
}
/*
    This will send the string information to server.
    first: send the size of data
    second: send the string data
**********************************************************/
void send_msg(int connectionSocket)
{

}
/*
    This will receive the string data from server
    first: receive the size of data
    second: receive the string data
********************************************************/
void receive_text_msg(int connectionSocket)
{
    char *text_buffer;
    int text_length, total_received = 0, received, index = 0;

    if(recv(connectionSocket, &text_length, sizeof(text_length), 0) < 0){
        fprintf(stderr, "ERROR reading from socket");
        exit(1);
    }
    fprintf(stdout, "Server: text length: %d\n", text_length);
    fflush(stdout);


    text_buffer = (char *)calloc(text_length+1, sizeof(char));
    memset(text_buffer, '\0', text_length+1);

    int is_done = 0;
    int data_size;

    while(!is_done){
        if(text_length-total_received >= 1000){
            data_size = 1000;
        }else{
            data_size = text_length-total_received;
        }
        received = recv(connectionSocket, text_buffer+index, data_size, 0);
        if(received<0){
           fprintf(stderr, "ERROR reading from socket");
           exit(1);
        }
        total_received+=received;
        if(total_received >= text_length){
            is_done = 1;
        }
        index+=received;
        fprintf(stdout, "Server: received %d\n", received);
        fflush(stdout);
    }
    fprintf(stdout,"Server: text\n %s\n", text_buffer);
    fflush(stdout);
}
void receive_key_msg(int connectionSocket)
{
    char *key_buffer;
    int key_length, total_received = 0, received, index = 0;

    if(recv(connectionSocket, &key_length, sizeof(key_length), 0) < 0){
        fprintf(stderr, "ERROR reading from socket");
        exit(1);
    }
    fprintf(stdout, "Server: key length: %d\n", key_length);
    fflush(stdout);



    key_buffer = (char *)calloc(key_length+1, sizeof(char));
    memset(key_buffer, '\0', key_length+1);

    int is_done = 0;
    int data_size;

    while(!is_done){
        if(key_length-total_received >= 1000){
            data_size = 1000;
        }else{
            data_size = key_length-total_received;
        }
        received = recv(connectionSocket, key_buffer+index, data_size, 0);
        if(received<0){
           fprintf(stderr, "ERROR reading from socket");
           exit(1);
        }
        total_received+=received;
        if(total_received >= key_length){
            is_done = 1;
        }
        index+=received;
        fprintf(stdout, "Server: received %d\n", received);
        fflush(stdout);
    }
    fprintf(stdout,"Server: key\n %s\n", key_buffer);
    fflush(stdout);
}
