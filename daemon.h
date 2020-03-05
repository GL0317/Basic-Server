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

struct server;


int checkInput(char *port);
void setUpAddress(struct server *bg);
void setUpSocket(struct server *bg);

#endif
