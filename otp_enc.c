/* filename: otp_enc.c
 *
 * SYNOPSIS:
 *
 * DESCRIPTION:
 *
 * AUTHOR: Gerson Lindor Jr. (lindorg@oregonstate.edu)
 * DATE CREATED: March 5, 2020
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

    memset(serverName, '\0', 10);
    memset(plaintext, '\0', STR_MAX);
    memset(ciphertext, '\0', STR_MAX);
    memset(mykey, '\0', STR_MAX);
    // get the plaintext from file
    readFile(argv[1], plaintext);
    // get the key from file
    readFile(argv[2], mykey);
    // verify command line inputs
    if (checkInput(plaintext, mykey, argv[3], argc)) {
        // setup the server address struct
        setUpAddress(enc, argv[3], "localhost");
        // setup the socket and connect to the server
        setUpConnect(enc);
        // receive the name of the server, and verify its otp_enc_d
        recvMsg(enc, serverName);
        if (strcmp(serverName, "otp_enc_d") != 0) {
            fprintf(stderr, "Error could not contact otp_enc_d on port %d\n", getPortNumber(enc));
            closeClient(enc);
            exit(2);
        }
 //printf("Received this message from server: %s\n", serverName);
        // send the key
        sendMsg(enc, mykey);
        // send the plaintext
        sendMsg(enc, plaintext);
        // receive the ciphertext
        recvMsg(enc, ciphertext);
        // close the socket
        closeClient(enc);
    }

    /****** test ******/
    printf("CLIENT: ciphertext = %s\n", ciphertext);
    /*******************/
    if (enc) { free(enc); enc = NULL; }


    return 0;
}

