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

struct clientServer;

int checkInput(char *text, char *key, char *port, int arg);
void setUpAddress(struct clientServer *client);
void setUpConnect(struct clientServer *client);


#endif
