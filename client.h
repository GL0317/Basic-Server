/* filename: client.h
 *
 * SYNOPSIS:
 *
 * DESCRIPTION:
 *
 * AUTHOR: Gerson Lindor Jr. (lindorg@oregonstate.edu)
 * DATE CREATED: March 5, 2020
 * DATE LAST MODIFIED:
 */


#ifndef CLIENT_H
#define CLIENT_H

const int STR_MAX;

struct clientServer;

struct clientServer *getClient();
void readFile(char *filepath, char *text);
int checkInput(char *text, char *key, char *port, int arg);
void setUpAddress(struct clientServer *client, char *port, char *hostname);
void setUpConnect(struct clientServer *client);
void recvMsg(struct clientServer *client, char *completeMsg);
void closeClient(struct clientServer *client);
void sendMsg(struct clientServer *client, char *msg);
int getPortNumber(struct clientServer *client);


#endif
