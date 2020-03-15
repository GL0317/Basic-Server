/*filename: daemon.c
 *
 * DESCRIPTION:
 * This file consists of operations that allow a server to act as a
 * daemon, and connect to multiple services. This file is implemented by
 * the otp_dec_d.c and otp_enc_d.c files.
 *
 * AUTHOR: Gerson Lindor Jr. (lindorg@oregonstate.edu)
 * DATE CREATED: March 5, 2020
 * DATE LAST MODIFIED: March 14, 2020
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
#include <arpa/inet.h>


struct server {
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    socklen_t sizeOfClientInfo;
    int listenSocketFD;
    int establishedConnectionFD;
    int port;
};


// receives the size of a string from a connected server
int recvSize(struct server *bg) {
    int size = 0;
    int result, flag;

    uint32_t num = htonl(size);
    flag = recv(bg->establishedConnectionFD, &num, sizeof(uint32_t), 0);
    if (flag < sizeof(uint32_t)) {
        fprintf(stderr, "SERVER: Warning not all size is received\n");
    }
    // change string to integer
    result =ntohl(num);
    return result;
}


/* Sends the size of a message to a server
 *@precondition:  The message must already exists and it's length calculated
  @postcondition: sends the integer value of size 4 bytes to the connected server
 */
void sendSize(struct server *bg, int size) {
    int sent;

    uint32_t num = htonl(size);
    sent = send(bg->establishedConnectionFD, &num, sizeof(uint32_t), 0);
    if (sent < 0) {
        fprintf(stderr, "SERVER: error writing to socket\n");
    }
}


// closes the established socket connection with a server
void disconnect(struct server *bg) {
    close(bg->establishedConnectionFD);
}


// Creates a Map of characters to use as a legend to encypt and decrypt data
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


// Searches the map for matching characters, and returns the location
// of that character in the map
int getLetterLocation(char value, char map[], int size) {
    int location = -1;
    int index = 0;

    while (index < size && location == -1) {
        if (value == map[index]) {
            location = index;
        }
        ++index;
    } 
    return location;
}


/*Performs encyption or decryption of data
 * @precondition:  the option parameter determines how data is handled. When its 0,
 *                the data is encypted, otherwise it's decrypted.
 *                All strings in the parameter cannot be NULL
 * @postcondtion:  stores encrypted or decrypted data into the secureText parameter
 */
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
}


/* Receives a message from a connecting server
 *@precondition: This function must know the size of the message that is being sent
  @postcondition: Ensures that all the contents of the message is received 
                  according to the size given
 */
void recvMsg(struct server *bg, char *completeMsg, int size) {
    int buffer;
    int result, diff, adjust = 10;
    int total = 0, copySize = size;

    if (adjust > size) {
        adjust = size;
    }
    // clear the message
    while (total < size) {
        buffer = adjust;
        char reader[buffer];
        // clear the reader
        memset(reader, '\0', buffer);
        // get the next chunk
        result = recv(bg->establishedConnectionFD, reader, buffer - 1, 0);
        // add that chunk to the complete message
        strcat(completeMsg, reader);
        total += result;
        copySize -= result;
        // check for errors
        if (result == -1) {
            fprintf(stderr, "SERVER: Error receiving message.\n");
            break;
        }
        if (buffer > copySize) {
            diff = buffer - copySize;
            adjust = buffer - diff + 1;
        }
        if (result == 0) break;
    }
}


/* Sends a message to a connecting server
 *@precondition: msg cannot be null, and the client must connected to the server
  @postcondition: Sends a message to the server, it does not verify that the server
  received the message
 */
void sendMsg(struct server *bg, char *msg) {
    int checkSend = -5;
    int written;

    // send message to server
    written = send(bg->establishedConnectionFD, msg, strlen(msg), 0);
    if (written < 0) {
        fprintf(stderr, "SERVER: error writing to socket\n");
        close(bg->establishedConnectionFD);
    }
    do {
        // check the send buffer for this socket
        ioctl(bg->establishedConnectionFD, TIOCOUTQ, &checkSend);
    } while (checkSend > 0);
    // check for an error
    if (checkSend < 0) {
        fprintf(stderr, "SERVER: ioctl error.\n");
    }
}


// The server stops listening for other servers to connect to
void closeServer(struct server *bg) {
    close(bg->listenSocketFD);
}
    

// Accepts the connection to another server
int acceptConnection(struct server *bg) {
    // get the size of the address for the client that will connect
    bg->sizeOfClientInfo = sizeof(bg->clientAddress);
    // accept connection
    bg->establishedConnectionFD = accept(bg->listenSocketFD, (struct sockaddr *)&bg->clientAddress, &bg->sizeOfClientInfo);
    if (bg->establishedConnectionFD < 0) {
        fprintf(stderr, "SERVER: Error on accept.\n");
        return 0;
    }
    return 1;
}


// returns the struct that contains data member needed to connect to a server
struct server *getServer() {
    struct server *aServer = (struct server *)malloc(sizeof(struct server));
    assert(aServer != 0);
    return aServer;
}


// Sets up the server address struct
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


// Creates a socket and begins listening for other servers to connect to 
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
