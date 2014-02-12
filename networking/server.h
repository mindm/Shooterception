/* Joonas Blomberg 0342152 <joonas.blomberg@lut.fi>
 * server.h
 *
 *
 *
 */
#ifndef SERVER_H_
#define SERVER_H_

#define MAXDATASIZE 512
#define BACKLOG 10
#define MAX_PLAYERS 4

#include "generic.h"

// The playerinfo struct
typedef struct Player
{
	int number;
	char address[INET6_ADDRSTRLEN];
	int port;
	int status; // Ready: 1 not: 0
	struct sockaddr_storage their_addr;
	int addr_size;

} Player;

void *get_in_addr(struct sockaddr *sa);
int get_in_port(struct sockaddr *sa);


void send_ok(Player player, int sock, int numplayers);
void send_turn(Player player, int sock);
void send_start(Player *players, int sock, int numplayers);
void send_area(Player *players, int sock, int numplayers, struct Field gamefield);
void send_win(Player *players, int sock, int numplayers, int winner);
int all_ready(Player *players, int numplayers);

#endif /* SERVER_H_ */
