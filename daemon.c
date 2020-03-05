/*filename: daemon.c
 * SYNOPSIS:
 *
 * DESCRIPTION:
 *
 * AUTHOR: Gerson Lindor Jr. (lindorg@oregonstate.edu)
 * DATE CREATED: March 5, 2020
 * DATE LAST MODIFIED:
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


struct server {
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    socklen_t sizeOfClientInfo;
    int listenSocketFD;
    int establishedConnectionFD;
    int port;
};



int checkInput(char *port) {
    int flag = 0;

    // verify that port is a number

    return flag;
}


//
void setUpAddress(struct server *bg) {
    // clear out address struct
    // get the port number and convert to an integer from string
    // create a network cable socket
    // store the port number
    // Any address is allowed for connection to this process

}


//
void setUpSocket(struct server *bg) {
    // set up the socket
    // create socket
    // Enable the socket to begin listening
}
