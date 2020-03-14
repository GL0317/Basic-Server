/*filename: daemon.h
 * SYNOPSIS:
 *
 * DESCRIPTION:
 *
 * AUTHOR: Gerson Lindor Jr. (lindorg@oregonstate.edu)
 * DATE CREATED: March 5,2020
 * DATE LAST MODIFIED:
 */



#ifndef DAEMON_H
#define DAEMON_H
#define _POSIX_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


struct server;
const int PROCESS;

struct server *getServer();
int checkInput(char *port);
void setUpAddress(struct server *bg, char *port);
void setUpSocket(struct server *bg, int numOfConnections);
int acceptConnection(struct server *bg);
void closeServer(struct server *bg);
void sendMsg(struct server *bg, char *msg);
void recvMsg(struct server *bg, char *completeMsg, int size);
void secureTransfer(char map[], int size, char *text, char *key, char *secureText, int option);
void createLetterMap(char map[], int size);
int getLetterLocation(int value, char map[], int size);
void disconnect(struct server *bg);
void changeToString(int num, char *strNum, int size);
int recvSize(struct server *bg);
void sendSize(struct server *bg, int size);

#endif
