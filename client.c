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
#include <netdb.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/ioctl.h>


const int STR_MAX = 70500;

struct clientServer {
    struct sockaddr_in serverAddress;
    struct hostent *serverHostInfo;
    int socketFD;
    int port;
};


void changeToString(int num, char *strNum, int size) {
    int flag = -2;
    flag = snprintf(strNum, size, "%d", num);
    if (!flag) {
        fprintf(stderr, "SERVER: error cannot convert number to string.\n");
    }
}


int getPortNumber(struct clientServer *client){
    return client->port;
}


void closeClient(struct clientServer *client){
    close(client->socketFD);
}


void sendMsg(struct clientServer *client, char *msg) {
    int checkSend = -5;
    int written;

    // send message to client
    written = send(client->socketFD, msg, strlen(msg), 0);
    if (written < 0) {
        fprintf(stderr, "CLIENT: error writing to socket\n");
    }
    do {
        // check the send buffer for this socket
        ioctl(client->socketFD, TIOCOUTQ, &checkSend);
/////        printf("CLIENT(58)checksend: %d\n", checkSend); /* DELETE WHEN WORKING */
    } while (checkSend > 0);
    // check for an error
    if (checkSend < 0) {
        fprintf(stderr, "SERVER: ioctl error.\n");
    }
}

int recvSize(struct clientServer *client) {
    char reader[10];
    int result, flag;

    memset(reader, '\0', 10);
    flag = recv(client->socketFD, reader, sizeof(reader)-1, 0);
    if (flag < strlen(reader)) {
        fprintf(stderr, "CLIENT: Warning not all data written to socket\n");
    }
    // change string to integer
    result = atoi(reader);
    return result;
}


void recvMsg(struct clientServer *client, char *completeMsg, int size) {
    //char reader[10];
    char reader[size];
    int result, total = 0;

    // clear the message
    memset(completeMsg, '\0', sizeof(completeMsg));
    while (total < size) {
        // clear the reader
        memset(reader, '\0', sizeof(reader));
        // get the next chunk
        result = recv(client->socketFD, reader, sizeof(reader) - 1, 0);
        // add that chunk to the complete message
        strcat(completeMsg, reader);
        /*** printf("CLIENT(80): mesage received from SERVER: \"%s\", total: \"%s\"\n", reader, completeMsg); ****/
        total += result;
        // check for errors
        if (result == -1) {
            fprintf(stderr, "CLIENT: Error receiving message.\n");
            break;
        }
        if (result == 0) break;
    }
    // find terminal location
/////    location = strstr(completeMsg, "\n") - completeMsg;
    // wipe out terminal with '\0'
///////    printf("CLIENT(91) recvMSg() - location = %d\n", location); /////////////////////////////////////////////////////
//////    completeMsg[location] = '\0';
}


struct clientServer *getClient() {
    struct clientServer *client = (struct clientServer *)malloc(sizeof(struct clientServer));
    assert(client != 0);
    return client;
}


//
void readFile(char *filepath, char *text) {
    FILE *reader;

    // open file
    reader = fopen(filepath, "r");
    if (!reader) {
        fprintf(stderr, "CLIENT: Error cannot open file.\n");
        exit(1);
    }
    // read the first line from file
    fgets(text, STR_MAX, reader);
    // close file
    fclose(reader);
}


//
void setUpConnect(struct clientServer *client) {
    // set up the socket
    client->socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (client->socketFD < 0) {
        fprintf(stderr, "CLIENT: ERROR opening socket\n");
    }
    // connect to server
    if (connect(client->socketFD, (struct sockaddr*)&client->serverAddress, sizeof(client->serverAddress)) < 0) {
        fprintf(stderr, "CLIENT: ERROR connecting\n");
    }
}


//
void setUpAddress(struct clientServer *client, char *port, char *hostname) {
    int portNumber;

    // clear out address struct
    memset((char*)&client->serverAddress,'\0', sizeof(client->serverAddress));
    // get the port number and convert to an integer from string
    portNumber = atoi(port);
    // create a network cable socket
    client->serverAddress.sin_family = AF_INET;
    client->serverAddress.sin_port = htons(portNumber); // store the port number
    client->port = portNumber;
    // convert the machine name into a special form
    client->serverHostInfo = gethostbyname(hostname);
    if (client->serverHostInfo == NULL) {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
    }
    // copy in the address
    memcpy((char *)&client->serverAddress.sin_addr.s_addr, (char *)client->serverHostInfo->h_addr_list[0], client->serverHostInfo->h_length);
}


int checkInput(char *text, char *key, char *port, int arg) {
    int flag = 0;
    int keySize = 0;
    int textSize = 0;
    int portSize = 0;
    int min = 65, max = 90;
    int lowercaseMin = 97, lowercaseMax = 122;
    if (text && key && port) {
        keySize = strlen(key);
        textSize = strlen(text);
        portSize = strlen(port);
    } else {
        fprintf(stderr, "CLIENT: Error, missing argument\n");
        return flag;
    } 
    // check if key is shorter than text, and port is a number
    if ( keySize < textSize) {
        fprintf(stderr, "CLIENT: Error, check argument count, key size, or text size\n");
        return flag;
    } 
    // verify port is a number
    for (int i = 0; i < portSize; ++i) {
        if (!isdigit(port[i])) {
            fprintf(stderr, "CLIENT: Error, There's a letter %c in the port number\n", port[i]);
            return flag;
        }
    }
    // Check for bad characters in key
    for (int i = 0; i < keySize; ++i) {
        // remove the new line character
        if (key[i] == '\n') {
            key[i] = '\0';
        } else if (key[i] < min || key[i] > max) {
            if (key[i] != 32) {
                fprintf(stderr, "CLIENT: Error, bad character in key: %c\n", key[i]);
                return flag;
            }
        }
        // check plaintext, make sure it's all in caps
        if (i < textSize) {
            if (text[i] >= lowercaseMin && text[i] <= lowercaseMax) {
                fprintf(stderr, "CLIENT: Error, lower case character in plaintext: %c\n", text[i]);
                return flag;
            }
            if (text[i] == '\n') {
                text[i] = '\0';
            }
        }
    }

    flag = 1;
    return flag;
}

