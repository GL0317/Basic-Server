/* filename: client.h
 *
 * DESCRIPTION:
 * This file is an interface for implementing a client server
 *
 * AUTHOR: Gerson Lindor Jr. (lindorg@oregonstate.edu)
 * DATE CREATED: March 5, 2020
 * DATE LAST MODIFIED: March 14, 2020
 */


#ifndef CLIENT_H
#define CLIENT_H

const int STR_MAX;

struct clientServer;

struct clientServer *getClient();
void readFile(char *filepath, char *text);
int checkInput(char *text, char *key, char *port);
void setUpAddress(struct clientServer *client, char *port, char *hostname);
void setUpConnect(struct clientServer *client);
void recvMsg(struct clientServer *client, char *completeMsg, int size);
void closeClient(struct clientServer *client);
void sendMsg(struct clientServer *client, char *msg);
int getPortNumber(struct clientServer *client);
int recvSize(struct clientServer *client);
void sendSize(struct clientServer *client, int size);


#endif
