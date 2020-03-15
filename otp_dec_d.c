/* filename: otp_dec_d.c
 *
 * SYNOPSIS:
 *      otp_dec_d PORT
 *
 * DESCRIPTION:
 *      This program is a daemon that performs decription of data.  The daemon can service
 *      up to five servers concurrently.  This program is ment to run continuosly as a 
 *      background process, and it will stop running if an encrypting client attempts to
 *      connect with it (otp_enc).
 *
 * AUTHOR: Gerson Lindor Jr. (lindorg@oregonstate.edu)
 * DATE CREATED: March 13, 2020
 * DATE LAST MODIFIED: March 14, 2020
 */


#include "daemon.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const int MAX = 70500;
const int PROCESS = 5;

int main(int argc, char **argv) {
    struct server *decD = NULL;
    char plaintext[MAX];
    char mykey[MAX];
    char ciphertext[MAX];
    int mapSize = 27;
    char secretMap[mapSize];
    int textSize = -1;
    char strSize[6];
    int index = 0;
    pid_t child[PROCESS];
    int exitMethod;

    // set up 5 processes
    for (int i = 0; i < PROCESS; ++i) {
        child[i] = -2;
    }
    // initalize the secret map
    createLetterMap(secretMap, mapSize);
    decD = getServer();
    // setup the address struct for the server
    setUpAddress(decD, argv[1]);
    // set up the socket and begin listening
    setUpSocket(decD, 5);
    while(1) {
        // accept a connection, blocking if one is not available until one connects
        if (child[index] == -2 && acceptConnection(decD)) {
            // fork a new process to encrypt data
            child[index] = fork();
            switch(child[index]) {
                case -1:
                    fprintf(stderr, "Hull Breach\n");
                    exit(1);
                    break;
                case 0:
                    memset(mykey, '\0', MAX);
                    memset(ciphertext, '\0', MAX);
                    memset(strSize, '\0', 6);
                    // send size of server name
                    textSize = strlen("otp_dec_d");
                    sendSize(decD, textSize);
                    // send server name to client
                    sendMsg(decD, "otp_dec_d");
                    // receive size of key
                    textSize = recvSize(decD);
                    // receive the key from otp_dec
                    recvMsg(decD, mykey, textSize);
                    // receive size from ciphertext
                    textSize = recvSize(decD);
                    // receive the ciphertext from otp_dec
                    recvMsg(decD, ciphertext, textSize);
                    // decrypt the data
                    memset(plaintext, '\0', MAX);
                    secureTransfer(secretMap, mapSize, ciphertext, mykey, plaintext, 1);
                    // send size of plaintext
                    textSize = strlen(plaintext);
                    sendSize(decD, textSize);
                    // send plaintext to otp_enc
                    sendMsg(decD, plaintext);
                    // close established connection with the client
                    disconnect(decD);
                    exit(0);
                    break;
                 default:
                    // in parent process: don't wait for child
                    child[index] = waitpid(child[index], &exitMethod, WNOHANG);
                    ++index;
                    if (index >= PROCESS) { index = 0; }
                    // close established connection with the client
                    disconnect(decD);
                    break;
            }
        }
        // check the status of child process and reset the completed ones to -1
        for (int i = 0; i < PROCESS; ++i) {
            if (child[i] != -2) {
                if (waitpid(child[i], &exitMethod, WNOHANG) != 0) {
                    if (WIFEXITED(exitMethod) || WIFSIGNALED(exitMethod)){
                        child[i] = -2;
                     }
                }
            }
        }
    }
    // close the listening socket
    closeServer(decD);
    if (decD) { free(decD); decD = NULL; }
    return 0;
}

