/*
* CT30A5002 Games and Networking 2013-14
* Authors:
* Arttu Hanska (0342181) - arttu.hanska@lut.fi
* Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
* Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#ifndef CLIENT_H_
#define CLIENT_H_

#define MAXDATASIZE 512

#include "../generic.h"
#include "packets.h"

// The playerinfo struct
/*typedef struct player_n
{
	int number;
	char address[INET6_ADDRSTRLEN];
	int port;
	struct sockaddr_storage their_addr;
	int addr_size;
    
} player_n;*/ //tää löytyy genericistä

game* gameState;

char buf[MAXDATASIZE];

typedef struct playerNames {
    int playerCount;
    char name[4][16];
} playerNames;

char pl1[16] = { 0 }; char pl2[16] = { 0 }; char pl3[16] = { 0 }; char pl4[16] = { 0 };
playerNames pl_names;

typedef struct serverInfo {
    int id;
    int port;
	char game_name[16];
    char address[40];
} serverInfo;

serverInfo cl_serverList[3];

void *networking_thread(void *dest_addr);

void setPlayerNames(char* pl1, char* pl2, char* pl3, char* pl4);
void updateEnemyStates(struct SDLenemy* _enemy, int i);
void updatePlayerStates(struct SDLplayer* _player, int i);
playerNames *getPlayers();
void getGameList(void);
void getServerList(void);
void *get_in_addr(struct sockaddr *sa);

void sendJoinGame(int id);
void sendCreateGame(void);
void sendGameStart(void);
void sendClientState(struct SDLplayer*, int);
void sendJoinGame(int id);
void sendChatMsg(char *message);
void readParams(char *full_address, char *host, char *port);
char *createFullAddress(char *host, char *port);

#endif /* SERVER_H_ */
