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
int initial_contact(int connectionSocket);
void send_msg(int connectionSocket);
void receive_text_msg(int connectionSocket);
void receive_key_msg(int connectionSocket);

int main(int argc, char *argv[]){
    int connectionSocket, charsRead;
    char buffer[256];
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

        if(initial_contact(connectionSocket) == 0){
            close(connectionSocket);
            exit(2);
        }

        receive_text_msg(connectionSocket);

        /*
        // Get the message from the client and display it
        memset(buffer, '\0', 256);
        // Read the client's message from the socket
        charsRead = recv(connectionSocket, buffer, 255, 0);
        if (charsRead < 0){
            fprintf(stderr, "ERROR reading from socket");
            exit(1);
        }
        printf("SERVER: I received this from the client: \"%s\"\n", buffer);

        // Send a Success message back to the client
        charsRead = send(connectionSocket, "I am the server, and I got your message", 39, 0);
        if (charsRead < 0){
            fprintf(stderr, "ERROR writing to socket");
            exit(1);
        }
        */
        // Close the connection socket for this client
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
int initial_contact(int connectionSocket)
{
    char recv_name[256];
    char reply[2];
    if(recv(connectionSocket, recv_name, 255, 0) < 0){
        fprintf(stderr, "Server: Error Faild to receive an initial message from client\n");
        exit(1);
    }
    fprintf(stdout, "test on server: %s\n", recv_name);
    fflush(stdout);
    if(strcmp(recv_name, "enc_client\0") == 0){
        reply[0] = '1';
    }else{
        reply[0] = '0';
    }
    fprintf(stdout, "test on server: %s\n", reply);
    fflush(stdout);
    if(send(connectionSocket, reply, 2, 0) < 0){
        fprintf(stderr, "Server: Error Faild to send an initial message to server\n");
        exit(1);
    }
    return atoi(reply);
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
    char buffer[256];
    int check;
    while( check = recv(connectionSocket, buffer, 255, 0) > 0){
        if ( check < 0){
            fprintf(stderr, "ERROR reading from socket");
            exit(1);
        }

        fprintf(stdout, "test1 on server: %s\n", buffer);
        fflush(stdout);
    }


}
void receive_key_msg(int connectionSocket)
{

}