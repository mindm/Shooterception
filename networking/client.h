/* Joonas Blomberg 0342152 <joonas.blomberg@lut.fi>
 * client.h
 *
 *
 *
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include "generic.h"
#include <ncurses.h>

#define MAXDATASIZE 512
#define STDIN 0  // file descriptor for standard input
#define BUFFER 256 //define  bufferlen
#define LOGSIZE 9
#define MESSAGEBUFFER 50

void *get_in_addr(struct sockaddr *sa);
struct Field init_field(void);
void ui_draw_game(struct Field *gamefield);
int handle_in(char* message, int msglen);
void add_message(char* message, int msglen);

#endif
