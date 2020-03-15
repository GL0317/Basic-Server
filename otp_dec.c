/* filename: otp_dec.c
 *
 * SYNOPSIS:
 *      otp_dec CIPHERTEXT KEY PORT
 *
 * DESCRIPTION:
 *      This program receives the ciphertext and key as inputs sends it to a server that
 *      performs decryption, and it receives the plaintext from that server.
 *
 * AUTHOR: Gerson Lindor Jr. (lindorg@oregonstate.edu)
 * DATE CREATED: March 13, 2020
 * DATE LAST MODIFIED: March 14, 2020
 */


#include "client.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char **argv) {
    struct clientServer *dec = NULL;
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
    if (checkInput(ciphertext, mykey, argv[3])) {
        dec = getClient();
        // setup the server address struct
        setUpAddress(dec, argv[3], "localhost");
        // setup the socket and connect to the server
        setUpConnect(dec);
        // receive the size of incomming text
        textSize = recvSize(dec);
        // receive the name of the server, and verify its otp_dec_d
        recvMsg(dec, serverName, textSize);
        if (!strcmp(serverName, "otp_enc_d")) {
            fprintf(stderr, "Error otp_dec cannot use %s on port %d.\n", serverName, getPortNumber(dec));
            closeClient(dec);
            exit(2);
        }
        // send key size
        textSize = strlen(mykey);
        sendSize(dec, textSize);
        // send the key
        sendMsg(dec, mykey);
        // send ciphertext size
        textSize = strlen(ciphertext);
        sendSize(dec, textSize);
        // send the ciphertext
        sendMsg(dec, ciphertext);
        // receive the size of the plaintext
        textSize = recvSize(dec);
        // receive the plaintext
        recvMsg(dec, plaintext, textSize);
        // close the socket
        closeClient(dec);
        printf("%s\n", plaintext);
        if (dec) { free(dec); dec = NULL; }
    }
    return 0;
}

