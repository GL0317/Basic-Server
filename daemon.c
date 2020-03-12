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
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <assert.h>
#include <sys/ioctl.h>


struct server {
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    socklen_t sizeOfClientInfo;
    int listenSocketFD;
    int establishedConnectionFD;
    int port;
};


void disconnect(struct server *bg) {
    close(bg->establishedConnectionFD);
}


void createLetterMap(char map[], int size) {
    int index = 0;
    int filler = 65;
    // give first index ascii value of space
    map[index] = 32;
    // fill in the rest of the array from ascii values A - Z
    ++index;
    while (index < size) {
        map[index] = filler;
        ++filler;
        ++index;
    }
}


int getLetterLocation(char value, char map[], int size) {
    int location = -1;
    int index = 0;

    // printf("SERVER: search value = %c\n", value);
    while (index < size && location == -1) {
    // printf("SERVER: inside map index %d = %c\n", index, map[index]);

        if (value == map[index]) {
            location = index;
        }
        ++index;
    } 
//     printf("SERVER: search value = %c\n", value);
//    for (int i = 0; i < size; ++i) {
//        if (value == map[i]) {
//            location = i;
     printf("SERVER(71): inside map found index %d = %c\n", location, map[location]);
///            break;
///        }
///    }
    return location;
}


void secureTransfer(char map[], int size, char *text, char *key, char *secureText, int option){
    int keylocal = -1;
    int textlocal = -1;
    int textSize = strlen(text);
    int index;
    
    // search for text character in the map
    for (int i = 0; i < textSize; ++i) {
        keylocal = getLetterLocation(key[i], map, size);
        textlocal = getLetterLocation(text[i], map, size);
        if (keylocal == -1 || textlocal == -1) {
            fprintf(stderr, "SERVER: Cannot find characters in key or text\n");
            return;
        }
        if (option == 0) {
            // encrypt the text character
            index = (keylocal + textlocal) % size;
        } else {
            // decrypt the text character
            index = textlocal - keylocal;
            if (index < 0) { index += size; }
        }
        // add results to secure text
        secureText[i] = map[index];
    }
    // add a newline at the end as a delimiter for send() and recv()
//////////////////////////////////////////////////    secureText[textSize] = '\n';
    printf("SERVER(106): securetext = %s\n", secureText);
}


void recvMsg(struct server *bg, char *completeMsg) {
    char reader[10];
    int result, location;

    // clear the message
    memset(completeMsg, '\0', sizeof(completeMsg));
    while (strstr(completeMsg, "\n") == NULL) {
        // clear the reader
        memset(reader, '\0', sizeof(reader));
        // get the next chunk
        result = recv(bg->establishedConnectionFD, reader, sizeof(reader) - 1, 0);
        // add that chunk to the complete message
        strcat(completeMsg, reader);
        /*** printf("CLIENT: mesage received from SERVER: \"%s\", total: \"%s\"\n", reader, completeMsg); ****/
        // check for errors
        if (result == -1) {
            fprintf(stderr, "SERVER: Error receiving message.\n");
            break;
        }
        if (result == 0) break;
    }
    // find terminal location
    location = strstr(completeMsg, "\n") - completeMsg;
    // wipe out terminal with '\0'
    printf("SERVER(133) recvMSg() - location = %d\n", location); /////////////////////////////////////////////////////
    completeMsg[location] = '\0';
}


void sendMsg(struct server *bg, char *msg) {
    int checkSend = -5;
    int written;

    // send message to server
    written = send(bg->establishedConnectionFD, msg, strlen(msg), 0);
    if (written < 0) {
        fprintf(stderr, "SERVER: error writing to socket\n");
        close(bg->establishedConnectionFD);
    }
//    do {
//        // check the send buffer for this socket
//        ioctl(bg->establishedConnectionFD, TIOCOUTQ, &checkSend);
//        printf("SERVER(151) checksend: %d\n", checkSend); /* DELETE WHEN WORKING */
//    } while (checkSend > 0);
    // check for an error
//    if (checkSend < 0) {
//        fprintf(stderr, "SERVER: ioctl error.\n");
//    }
}



void closeServer(struct server *bg) {
    close(bg->listenSocketFD);
}
    

int acceptConnection(struct server *bg) {
    // get the size of the address for the client that will connect
    bg->sizeOfClientInfo = sizeof(bg->clientAddress);
    // accept connection
    bg->establishedConnectionFD = accept(bg->listenSocketFD, (struct sockaddr *)&bg->clientAddress, &bg->sizeOfClientInfo);
    if (bg->establishedConnectionFD < 0) {
        fprintf(stderr, "SERVER: Error on accept.\n");
        return 0;
    }
    printf("SERVER: Connected to client at port %d\n", ntohs(bg->clientAddress.sin_port));
    return 1;
}


struct server *getServer() {
    struct server *aServer = (struct server *)malloc(sizeof(struct server));
    assert(aServer != 0);
    return aServer;
}


int checkInput(char *port) {
    int flag = 1;
    int portSize = 0;

    if (!port) {
        return 0;
    }
    portSize = strlen(port);
    // verify port is a number
    for (int i = 0; i < portSize; ++i) {
        if (!isdigit(port[i])) {
            fprintf(stderr, "SERVER: Error, There's a letter %c in the port number\n", port[i]);
            flag = 0;
            return flag;
        }
    }
    return flag;
}


//
void setUpAddress(struct server *bg, char *port) {
    int portNumber;

    // clear out address struct
    memset((char*)&bg->serverAddress,'\0', sizeof(bg->serverAddress));
    // get the port number and convert to an integer from string
    portNumber = atoi(port);
    // create a network cable socket
    bg->serverAddress.sin_family = AF_INET;
    bg->serverAddress.sin_port = htons(portNumber); // store the port number
    // Any address is allowed for connection to this process
    bg->serverAddress.sin_addr.s_addr = INADDR_ANY;
}


//
void setUpSocket(struct server *bg, int numOfConnections) {
    // set up the socket
    bg->listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // create socket
    // Enable the socket to begin listening
    if (bind(bg->listenSocketFD, (struct sockaddr *)&bg->serverAddress, sizeof(bg->serverAddress)) < 0) {
        fprintf(stderr, "ERROR on binding\n");
        exit(1);
    }
    listen(bg->listenSocketFD, numOfConnections);
}
