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
struct Field init_field(void);
int is_linear_match(struct Field *gamefield, int x, int y, int stepx, int stepy);
int is_horizontal_match(struct Field *gamefield);
int is_vertical_match(struct Field *gamefield);
int is_diagonal_down_match(struct Field *gamefield);
int is_diagonal_up_match(struct Field *gamefield);
int win_condition_met(struct Field *gamefield);
int insert_to_column(int col, struct Field *gamefield, Player player);
void send_ok(Player player, int sock, int numplayers);
void send_turn(Player player, int sock);
void send_start(Player *players, int sock, int numplayers);
void send_area(Player *players, int sock, int numplayers, struct Field gamefield);
void send_win(Player *players, int sock, int numplayers, int winner);
int all_ready(Player *players, int numplayers);

#endif /* SERVER_H_ */
