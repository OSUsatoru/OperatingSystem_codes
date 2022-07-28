/*
    ONID: yamamsat
    email: yamamsat@oregonstate.edu

    NOTE: I avoid to use single-line comment because this was created in cpp
    If I remember correctly, some of old compiler display error message for this.

    Here is the links where I checked while I was making Programming memo

    reference:
    - for SIGCHLD
    https://www.ipa.go.jp/security/awareness/vendor/programmingv1/b07_04.html

    - get idea of server program
    http://research.nii.ac.jp/~ichiro/syspro98/server.html

    - get idea to make loop for recv
    https://www.mathkuro.com/network/socket/c-tcp-socket-sample/
**********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

char key_characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
char *text_buffer, *key_buffer, *decrypted_buffer;

/* http://research.nii.ac.jp/~ichiro/syspro98/server.html */
void catch_SIGCHLD(int signo);

void setupAddressStruct(struct sockaddr_in* address, int portNumber);
void childProcess(int connectionSocket);
void to_decrypt();
int initial_contact(int connectionSocket);
void send_result(int connectionSocket);
void receive_text_msg(int connectionSocket);
void receive_key_msg(int connectionSocket);

int main(int argc, char *argv[]){
    int connectionSocket, charsRead;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo = sizeof(clientAddress);

    /* catch signal when child executed */
    /*http://research.nii.ac.jp/~ichiro/syspro98/server.html*/
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = catch_SIGCHLD;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NOCLDSTOP | SA_RESTART;
    sigaction(SIGCHLD, &act, NULL);

    /* Check usage & args */
    if (argc < 2) {
        fprintf(stderr,"USAGE: %s port\n", argv[0]);
        exit(1);
    }

    /* Create the socket that will listen for connections */
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        fprintf(stderr, "Server: ERROR opening socket");
        exit(2);
    }

    /* Set up the address struct for the server socket */
    setupAddressStruct(&serverAddress, atoi(argv[1]));

    /* Associate the socket to the port */
    if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
        fprintf(stderr, "Server: ERROR on binding\n");
        exit(2);
    }

    /* Start listening for connetions. Allow up to 5 connections to queue up */
    listen(listenSocket, 5);


    /* Accept a connection, blocking if one is not available until one connects */
    while(1){
        /* Accept the connection request which creates a connection socket */
        connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
        if (connectionSocket < 0){
            fprintf(stderr, "Server: ERROR on accept\n");
            exit(2);
        }
        /*
        printf("SERVER: Connected to client running at host %d port %d\n",ntohs(clientAddress.sin_addr.s_addr), ntohs(clientAddress.sin_port));
        */

        pid_t spawnPid;

        spawnPid = fork();

        switch(spawnPid){
            case -1:
                fprintf(stderr, "Server: Error fork() failed\n");
                exit(1);
                break;
            case 0: /* This is on child process */
                /* Child should not do anything on ListenSocket */
                close(listenSocket);
                childProcess(connectionSocket);
                /* Close the connection socket for this client */
                close(connectionSocket);
                exit(0);
                break;
            default: /* parent proces */
                break;
        }
        /* close the connection socket */
        close(connectionSocket);
    }
    // Close the listening socket
    close(listenSocket);
    return 0;
}
/* https://www.ipa.go.jp/security/awareness/vendor/programmingv1/b07_04.html */
void catch_SIGCHLD(int signo)
{
    /* check the background process */
    pid_t child_pid = 0;
    do{
        int child_ret;
        child_pid = waitpid(-1, &child_ret, WNOHANG);
    }while(child_pid>0);
}

/* Set up the address struct for the server socket */
void setupAddressStruct(struct sockaddr_in* address, int portNumber){
  /* Clear out the address struct */
  memset((char*) address, '\0', sizeof(*address));

  /* The address should be network capable */
  address->sin_family = AF_INET;
  /* Store the port number */
  address->sin_port = htons(portNumber);
  /* Allow a client at any address to connect to this server */
  address->sin_addr.s_addr = INADDR_ANY;
}
/*
    The child processes will execute
    this funciton to decript string
**********************************************************/
void childProcess(int connectionSocket)
{
    /* check the connection is valid */
    if(initial_contact(connectionSocket) == 0){
        close(connectionSocket);
        exit(2);
    }
    /* receive the text stirng form client */
    receive_text_msg(connectionSocket);
    /* receive the key string from client */
    receive_key_msg(connectionSocket);

    /*
    fprintf(stdout, "Server: text\n%s\n", text_buffer);
    fprintf(stdout, "Server: key\n%s\n", key_buffer);
    fprintf(stdout, "tested\n");
    fflush(stdout);
    */
    /* to decript text stirng using key stirng */
    to_decrypt();
    /* send decripted stirng to client */
    send_result(connectionSocket);
    /*
    fprintf(stdout, "tested\n");
    fflush(stdout);
    */

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
    int reply;
    memset(recv_name, '\0', 256);

    /* receive the system name from client */
    if(recv(connectionSocket, recv_name, 255, 0) < 0){
        fprintf(stderr, "Server: Error Faild to receive an initial message from client\n");
        exit(2);
    }
    /*
    fprintf(stdout, "test on server: %s\n", recv_name);
    fflush(stdout);
    */

    if(strcmp(recv_name, "dec_client\0") == 0){
        reply = 1;
    }else{
        reply = 0;
    }
    /*
    fprintf(stdout, "test on server: %d\n", reply);
    fflush(stdout);
    */
    /* send the reply to client */
    if(send(connectionSocket, &reply, sizeof(reply), 0) < 0){
        fprintf(stderr, "Server: Error Faild to send an initial message to client\n");
        exit(2);
    }
    return reply;
}
/*
    This function decript text stirng by using key string
***************************************************************/
void to_decrypt()
{
    int text_size = strlen(text_buffer);
    decrypted_buffer = (char *)calloc(text_size+1, sizeof(char));
    memset(decrypted_buffer, '\0', text_size+1);
    /* Do not need to check the last null character */
    for(int i = 0; i < text_size; ++i){
        int text_index = strchr(key_characters,text_buffer[i]) - key_characters, key_index = strchr(key_characters,key_buffer[i]) - key_characters;
        decrypted_buffer[i] = key_characters[(strlen(key_characters) + text_index - key_index) % strlen(key_characters)];
    }
    /*
    fprintf(stdout, "decripted: %s\n", decrypted_buffer);
    fflush(stdout);
    */
}
/*
    This will send the string information to server.
    first: send the size of data
    second: send the string data
**********************************************************/
void send_result(int connectionSocket)
{
    int dec_size = strlen(decrypted_buffer);
    /* send the size of decripted strign */
    if(send(connectionSocket, &dec_size, sizeof(dec_size), 0) < 0){
        fprintf(stderr, "Server: Error Faild to send an initial message to Client\n");
        exit(2);
    }
    /*
    fprintf(stdout, "strlen: %d\n", strlen(key_buffer));
    fflush(stdout);
    */
    /* send the decripted strign */
    if(send(connectionSocket, decrypted_buffer, dec_size, 0) < 0){
        fprintf(stderr, "Server: Error Faild to send an initial message to Client\n");
        exit(2);
    }
}
/*
    This will receive the string data from server
    first: receive the size of data
    second: receive the string data
********************************************************/
void receive_text_msg(int connectionSocket)
{
    int text_length, total_received = 0, received, index = 0;
    /* receive the size of text string  */
    if(recv(connectionSocket, &text_length, sizeof(text_length), 0) < 0){
        fprintf(stderr, "Server: ERROR reading from client\n");
        exit(2);
    }
    /*
    fprintf(stdout, "Server: text length: %d\n", text_length);
    fflush(stdout);
    */

    /* allocate memory */
    text_buffer = (char *)calloc(text_length+1, sizeof(char));
    memset(text_buffer, '\0', text_length+1);

    int is_done = 0;
    int data_size;
    /*
        loop until done
        This receive every 5000 characters (you can change the size)
    ******************************************************/
    while(!is_done){
        if(text_length-total_received >= 5000){
            data_size = 5000;
        }else{
            data_size = text_length-total_received;
        }
        received = recv(connectionSocket, text_buffer+index, data_size, 0);
        if(received<0){
           fprintf(stderr, "Server: ERROR reading from client\n");
           exit(2);
        }
        total_received+=received;
        if(total_received >= text_length){
            is_done = 1;
        }
        index+=received;
        /*
        fprintf(stdout, "Server: received %d\n", received);
        fflush(stdout);
        */
    }

}
/*
    This will receive the string data from server
    first: receive the size of data
    second: receive the string data
********************************************************/
void receive_key_msg(int connectionSocket)
{

    int key_length, total_received = 0, received, index = 0;
    /* receive the size of key string  */
    if(recv(connectionSocket, &key_length, sizeof(key_length), 0) < 0){
        fprintf(stderr, "ERROR reading from socket\n");
        exit(2);
    }
    /*
    fprintf(stdout, "Server: key length: %d\n", key_length);
    fflush(stdout);
    */

    /* allocate memory */
    key_buffer = (char *)calloc(key_length+1, sizeof(char));
    memset(key_buffer, '\0', key_length+1);

    int is_done = 0;
    int data_size;
    /*
        loop until done
        This receive every 5000 characters (you can change the size)
    ******************************************************/
    while(!is_done){
        if(key_length-total_received >= 5000){
            data_size = 5000;
        }else{
            data_size = key_length-total_received;
        }
        received = recv(connectionSocket, key_buffer+index, data_size, 0);
        if(received<0){
           fprintf(stderr, "ERROR reading from socket\n");
           exit(2);
        }
        total_received+=received;
        if(total_received >= key_length){
            is_done = 1;
        }
        index+=received;
        /*
        fprintf(stdout, "Server: received %d\n", received);
        fflush(stdout);
        */
    }

}
