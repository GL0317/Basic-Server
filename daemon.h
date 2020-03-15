/*filename: daemon.h
 * SYNOPSIS:
 *
 * DESCRIPTION:
 *
 * AUTHOR: Gerson Lindor Jr. (lindorg@oregonstate.edu)
 * DATE CREATED: March 5,2020
 * DATE LAST MODIFIED: March 14, 2020
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
int recvSize(struct server *bg);
void sendSize(struct server *bg, int size);

#endif
