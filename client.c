/* filename: client.c
 *
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
#include "client.h"
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


struct clientServer {
    struct sockaddr_in serverAddress;
    struct hostent *serverHostInfo;
    int socketFD;
    int port;
};


//
void setUpConnect(struct clientServer *client) {
    // set up the socket
    // connect to server
}


//
void setUpAddress(struct clientServer *client) {
    // clear out address struct
    // get the port number and convert to an integer from string
    // create a network cable socket
    // convert the machine name into a special form

}


int checkInput(char *text, char *key, char *port, int arg) {
    int flag = 0;
    // Check for bad characters in key
    // check if key is shorter than text
    // verify that port is a number
    // check argument count
    return flag;
}

