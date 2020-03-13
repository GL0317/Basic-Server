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
        textSize = strlen(mykey);
    printf("Begining key size = %d\n", textSize);
    if (checkInput(plaintext, mykey, argv[3], argc)) {
        // setup the server address struct
        setUpAddress(enc, argv[3], "localhost");
        // setup the socket and connect to the server
        setUpConnect(enc);
        // receive the size of incomming text
        textSize = recvSize(enc);
        printf("OTP_ENC(43) size of msg text = %d\n", textSize); ///////////////////////////
        // receive the name of the server, and verify its otp_enc_d
        recvMsg(enc, serverName, textSize);
        printf("OTP_ENC(46) serverName = %s\n", serverName); ///////////////////////////
        if (strcmp(serverName, "otp_enc_d") != 0) {
            fprintf(stderr, "Error could not contact otp_enc_d on port %d\n", getPortNumber(enc));
            closeClient(enc);
            exit(2);
        }
 printf("Received this message from server: %s\n", serverName);
        // send key size
        textSize = strlen(mykey);
        changeToString(textSize, strSize, 6);
        sendMsg(enc, strSize);
        // send the key
        printf("OTP_ENC(63) just sent key size = %s\n", strSize);
        sendMsg(enc, mykey);
        // send plaintext size
        textSize = strlen(plaintext);
        changeToString(textSize, strSize, 6);
        printf("OTP_ENC(63) just sent plaintext size = %s\n", strSize);
        sendMsg(enc, strSize);
        // send the plaintext
        sendMsg(enc, plaintext);
        // receive the size of the ciphertext
        textSize = recvSize(enc);
        printf("OTP_ENC(68) size of ciphertext = %d\n", textSize); ///////////////
        // receive the ciphertext
        recvMsg(enc, ciphertext, textSize);
        // close the socket
        closeClient(enc);
    }
    printf("%s\n", ciphertext);
    if (enc) { free(enc); enc = NULL; }
    return 0;
}

