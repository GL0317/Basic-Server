/* filename: otp_dec.c
 *
 * SYNOPSIS:
 *
 * DESCRIPTION:
 *
 * AUTHOR: Gerson Lindor Jr. (lindorg@oregonstate.edu)
 * DATE CREATED: March 13, 2020
 * DATE LAST MODIFIED:
 */


#include "client.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char **argv) {
    struct clientServer *enc = getClient();
    char mykey[STR_MAX];
    char plaintext[STR_MAX];
    char ciphertext[STR_MAX];
    char serverName[10];
    int textSize = -1;
    char strSize[6];

    memset(serverName, '\0', 10);
    memset(strSize, '\0', 6);
    memset(plaintext, '\0', STR_MAX);
    memset(ciphertext, '\0', STR_MAX);
    memset(mykey, '\0', STR_MAX);
    // get the plaintext from file
    readFile(argv[1], ciphertext);
    // get the key from file
    readFile(argv[2], mykey);
    // verify command line inputs
    if (checkInput(ciphertext, mykey, argv[3], argc)) {
        // setup the server address struct
        setUpAddress(enc, argv[3], "localhost");
        // setup the socket and connect to the server
        setUpConnect(enc);
        // receive the size of incomming text
        textSize = recvSize(enc);
        // receive the name of the server, and verify its otp_enc_d
        recvMsg(enc, serverName, textSize);
        printf("daemon name: %s\n", serverName);
        if (strcmp(serverName, "otp_dec_d") != 0) {
            fprintf(stderr, "Error could not contact otp_dec_d on port %d\n", getPortNumber(enc));
            closeClient(enc);
            exit(2);
        }
printf("***** SENDING THE KEY ****\n"); 
        // send key size
        textSize = strlen(mykey);
        sendSize(enc, textSize);
printf("client send key size = %d\n", textSize); 
        // send the key
        sendMsg(enc, mykey);
printf("***** SENDING THE CIPHERTEXT ****\n"); 
        // send ciphertext size
        textSize = strlen(ciphertext);
        sendSize(enc, textSize);
printf("client send ciphertext size = %d\n", textSize); 
        // send the ciphertext
        sendMsg(enc, ciphertext);
        // receive the size of the plaintext
printf("***** RECEIVING THE PLAINTEXT ****\n"); 
        textSize = recvSize(enc);
printf("Client received plaintext size = %d\n", textSize);
        // receive the plaintext
        recvMsg(enc, plaintext, textSize);
        // close the socket
        closeClient(enc);
    }
    printf("%s\n", plaintext);
    if (enc) { free(enc); enc = NULL; }
    return 0;
}

