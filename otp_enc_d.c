/* filename: otp_enc_d.c
 *
 * SYNOPSIS:
 *
 * DESCRIPTION:
 *
 * AUTHOR: Gerson Lindor Jr. (lindorg@oregonstate.edu)
 * DATE CREATED: March 5, 2020
 * DATE LAST MODIFIED:
 */


#include "daemon.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const int MAX = 7500;

int main(int argc, char **argv) {
    struct server *encD = getServer();
    char plaintext[MAX];
    char mykey[MAX];
    char ciphertext[MAX];
    int mapSize = 27;
    char secretMap[mapSize];
    int insertNewLine = -1;

    // initalize the secret map
    createLetterMap(secretMap, mapSize);
    memset(mykey, '\0', MAX);
    memset(plaintext, '\0', MAX);
    if (checkInput(argv[1])) {
        // setup the address struct for the server
        setUpAddress(encD, argv[1]);
        // set up the socket and begin listening
        setUpSocket(encD, 5);
        while(1) {
            // accept a connection, blocking if one is not available until one connects
            if (acceptConnection(encD)) {
                // send server name to client
                sendMsg(encD, "otp_enc_d\n");
                // receive the key from otp_enc
                recvMsg(encD, mykey);
        /***/ printf("Received the key from CLIENT: %s\n", mykey);
                // receive the plaintext from otp_enc
                recvMsg(encD, plaintext);
        /***/ printf("Received the plaintext from CLIENT: %s\n", plaintext);
                // encrypt the data
                memset(ciphertext, '\0', MAX);
                secureTransfer(secretMap, mapSize, plaintext, mykey, ciphertext, 0);
                // add a new line character to ciphertext as a delimiter
                printf("SERVER(53) this is ciphertext: %s\n", ciphertext);
                insertNewLine = strlen(ciphertext);
                ciphertext[insertNewLine] = '\n';
                printf("SERVER(55) There must be a new line after this: %s", ciphertext);
                // send ciphertext to otp_enc
                sendMsg(encD, ciphertext);
////                sendMsg(encD, "ABCDE\n");
                // close established connection with the client
                disconnect(encD);
            }
        }
    }
    // close the listening socket
    closeServer(encD);
    if (encD) { free(encD); encD = NULL; }

    return 0;
}

