/* filename: otp_dec_d.c
 *
 * SYNOPSIS:
 *
 * DESCRIPTION:
 *
 * AUTHOR: Gerson Lindor Jr. (lindorg@oregonstate.edu)
 * DATE CREATED: March 13, 2020
 * DATE LAST MODIFIED:
 */


#include "daemon.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const int MAX = 70500;

int main(int argc, char **argv) {
    struct server *encD = getServer();
    char plaintext[MAX];
    char mykey[MAX];
    char ciphertext[MAX];
    int mapSize = 27;
    char secretMap[mapSize];
    int textSize = -1;
    char strSize[6];

    // initalize the secret map
    createLetterMap(secretMap, mapSize);
    if (checkInput(argv[1])) {
        // setup the address struct for the server
        setUpAddress(encD, argv[1]);
        // set up the socket and begin listening
        setUpSocket(encD, 5);
        while(1) {
            // accept a connection, blocking if one is not available until one connects
            if (acceptConnection(encD)) {
                memset(mykey, '\0', MAX);
                memset(ciphertext, '\0', MAX);
                memset(strSize, '\0', 6);
                // send size of server name
                textSize = strlen("otp_dec_d");
 ///               changeToString(textSize, strSize, 6);
                sendSize(encD, textSize);
printf("Server sends otp_dec_d size = %d\n", textSize);
                // send server name to client
                sendMsg(encD, "otp_dec_d");
// printf("--------- NOW RECEIVING MESSAGES FROM CLIENT--------\n");
                // receive size of key
                textSize = recvSize(encD);
//    printf("server received key size = %d\n", textSize); 
                // receive the key from otp_dec
                recvMsg(encD, mykey, textSize);
                // receive size from ciphertext
                textSize = recvSize(encD);
//    printf("server received plaintext size = %d\n", textSize); 
                // receive the ciphertext from otp_dec
                recvMsg(encD, ciphertext, textSize);
                // decrypt the data
                memset(plaintext, '\0', MAX);
                secureTransfer(secretMap, mapSize, ciphertext, mykey, plaintext, 1);
                // send size of plaintext
//                do {
                textSize = strlen(plaintext);
////                changeToString(textSize, strSize, 6);
//printf("Server sends ciphertext size:\n");
                sendSize(encD, textSize);
//                } while (textSize = 0);
printf("Server sends plaintext size = %d\n", textSize);
                // send plaintext to otp_enc
                sendMsg(encD, plaintext);
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

