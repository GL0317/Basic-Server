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

const int MAX = 70500;
const int PROCESS = 5;

int main(int argc, char **argv) {
    struct server *encD = getServer();
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
    if (checkInput(argv[1])) {
        // setup the address struct for the server
        setUpAddress(encD, argv[1]);
        // set up the socket and begin listening
        setUpSocket(encD, 5);
        while(1) {
            // search through processes 
            // accept a connection, blocking if one is not available until one connects
            if (child[index] == -2 && acceptConnection(encD)) {
                // fork a new process to encrypt data
                child[index] = fork();
                switch(child[index]) {
                    case -1:
                        fprintf(stderr, "Hull Breach\n");
                        exit(1);
                        break;
                    case 0:
                        memset(mykey, '\0', MAX);
                        memset(plaintext, '\0', MAX);
                        memset(strSize, '\0', 6);
                        // send size of server name
                        textSize = strlen("otp_enc_d");
                        sendSize(encD, textSize);
                        // send server name to client
                        sendMsg(encD, "otp_enc_d");
                        // receive size of key
                        textSize = recvSize(encD);
                        // receive the key from otp_enc
                        recvMsg(encD, mykey, textSize);
                        // receive size from plaintext
                        textSize = recvSize(encD);
                        // receive the plaintext from otp_enc
                        recvMsg(encD, plaintext, textSize);
                        // encrypt the data
                        memset(ciphertext, '\0', MAX);
                        secureTransfer(secretMap, mapSize, plaintext, mykey, ciphertext, 0);
                        // send size of ciphertext
                        textSize = strlen(ciphertext);
                        sendSize(encD, textSize);
                        // send ciphertext to otp_enc
                        sendMsg(encD, ciphertext);
                        disconnect(encD);
                        break;
                    default:
                        // in parent process: don't wait for child
                        child[index] = waitpid(child[index], &exitMethod, WNOHANG);
                        ++index;
                        if (index >= PROCESS) { index = 0; }
                        // close established connection with the client
                        disconnect(encD);
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
    }
    // close the listening socket
    closeServer(encD);
    if (encD) { free(encD); encD = NULL; }
    return 0;
}

