/* filename: otp_enc.c
 *
 * SYNOPSIS:
 *      otp_enc PLAINTEXT KEY PORT
 *
 * DESCRIPTION:
 *      This program sends the plaintext and the key to an encrypting server, and 
 *      receives the ciphertext from that server.
 *
 * AUTHOR: Gerson Lindor Jr. (lindorg@oregonstate.edu)
 * DATE CREATED: March 5, 2020
 * DATE LAST MODIFIED: March 14, 2020
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
    readFile(argv[1], plaintext);
    // get the key from file
    readFile(argv[2], mykey);
    // verify command line inputs
    if (checkInput(plaintext, mykey, argv[3])) {
        enc = getClient();
        // setup the server address struct
        setUpAddress(enc, argv[3], "localhost");
        // setup the socket and connect to the server
        setUpConnect(enc);
        // receive the size of incomming text
        textSize = recvSize(enc);
        // receive the name of the server, and verify its otp_enc_d
        recvMsg(enc, serverName, textSize);
        if (!strcmp(serverName, "otp_dec_d")) {
            fprintf(stderr, "Error otp_enc cannot use %s on port %d.\n", serverName, getPortNumber(enc));
            closeClient(enc);
            exit(2);
        }
        // send key size
        textSize = strlen(mykey);
        sendSize(enc, textSize);
        // send the key
        sendMsg(enc, mykey);
        // send plaintext size
        textSize = strlen(plaintext);
        sendSize(enc, textSize);
        // send the plaintext
        sendMsg(enc, plaintext);
        // receive the size of the ciphertext
        textSize = recvSize(enc);
        // receive the ciphertext
        recvMsg(enc, ciphertext, textSize);
        // close the socket
        closeClient(enc);
        printf("%s\n", ciphertext);
        if (enc) { free(enc); enc = NULL; }
    }
    return 0;
}

