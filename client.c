/* filename: client.c
 *
 * DESCRIPTION:
 * This file provides functions that allows a server to act as a client.
 * It sends the plaintext or ciphertext and a key to a server and received in 
 * return the ciphertext of a sent plaintext or the plaintext or a sent ciphertext
 * This file is impletemented by otp_enc.c and otp_enc.d
 *
 * AUTHOR: Gerson Lindor Jr. (lindorg@oregonstate.edu)
 * DATE CREATED: March 5, 2020
 * DATE LAST MODIFIED: March 14, 2020
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
#include <arpa/inet.h>


const int STR_MAX = 70500;

struct clientServer {
    struct sockaddr_in serverAddress;
    struct hostent *serverHostInfo;
    int socketFD;
    int port;
};


// Returns the port number the client server
int getPortNumber(struct clientServer *client){
    return client->port;
}


// Wrapper function for closing a socket
void closeClient(struct clientServer *client){
    close(client->socketFD);
}


/* Sends a message to a connecting server
 *@precondition: msg cannot be null, and the client must connected to the server
  @postcondition: Sends a message to the server, it does not verify that the server
  received the message
 */
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
    } while (checkSend > 0);
    // check for an error
    if (checkSend < 0) {
        fprintf(stderr, "SERVER: ioctl error.\n");
    }
}


/* Sends the size of a message to a server
 *@precondition:  The message must already exists and it's length calculated
  @postcondition: sends the integer value of size 4 bytes to the connected server
 */
void sendSize(struct clientServer *client, int size) {
    int sent;

    uint32_t num = htonl(size);
    sent = send(client->socketFD, &num, sizeof(uint32_t), 0);
    if (sent < 0) {
        fprintf(stderr, "CLIENT: error writing to socket\n");
    }
}


// receives the size of a string from a connected server
int recvSize(struct clientServer *client) {
    int size = 0;
    int result, flag;

    uint32_t num = htonl(size);
    flag = recv(client->socketFD, &num, sizeof(uint32_t), 0);
    if (flag < sizeof(uint32_t)) {
        fprintf(stderr, "CLIENT: Warning did not receive all of data\n");
    }
    // change string to integer
    result = ntohl(num);
    return result;
}


/* Receives a message from a connecting server
 *@precondition: This function must know the size of the message that is being sent
  @postcondition: Ensures that all the contents of the message is received 
                  according to the size given
 */
void recvMsg(struct clientServer *client, char *completeMsg, int size) {
    int buffer; 
    int result, total = 0;
    int diff, adjust = 10;
    int copySize = size;

    if (adjust > size) {
        adjust = size;
    }
    // clear the message
    memset(completeMsg, '\0', sizeof(completeMsg));
    while (total < size) {
        buffer = adjust;
        char reader[buffer];
        // clear the reader
        memset(reader, '\0', buffer);
        // get the next chunk
        result = recv(client->socketFD, reader, buffer - 1, 0);
        // add that chunk to the complete message
        strcat(completeMsg, reader);
        total += result;
        copySize = copySize - result;
        // check for errors
        if (result == -1) {
            fprintf(stderr, "CLIENT: Error receiving message.\n");
            break;
        }
        // adjust the buffer so that only the exact ammount to prevent an over-read
        if (buffer > copySize) {
            diff = buffer - copySize;
            adjust = buffer - diff + 1;
        }
        if (result == 0) break;
    }
}


// returns the struct that contains data member needed to connect to a server
struct clientServer *getClient() {
    struct clientServer *client = (struct clientServer *)malloc(sizeof(struct clientServer));
    assert(client != 0);
    return client;
}


// Only reads the first line of a file and stores it the text variable
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


// Sets up the socket and connects to a server
void setUpConnect(struct clientServer *client) {
    // set up the socket
    client->socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (client->socketFD < 0) {
        fprintf(stderr, "CLIENT: ERROR opening socket\n");
    }
    // connect to server
    if (connect(client->socketFD, (struct sockaddr*)&client->serverAddress, sizeof(client->serverAddress)) < 0) {
        fprintf(stderr, "CLIENT: ERROR connecting\n");
        exit(1);
    }
}


// Sets up the server address struct
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
        exit(1);
    }
    // copy in the address
    memcpy((char *)&client->serverAddress.sin_addr.s_addr, (char *)client->serverHostInfo->h_addr_list[0], client->serverHostInfo->h_length);
}


// Verifies that the text and key have authorized characters.
// It ensures that port is a number. 
int checkInput(char *text, char *key, char *port) {
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
        fprintf(stderr, "CLIENT: The key is too short\n");
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
                fprintf(stderr, "CLIENT: Error, bad character %c in plaintext.\n", text[i]);
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

