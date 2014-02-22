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


typedef struct playerNames {
    int playerCount;
    char name[4][16];
} playerNames;

struct serverInfo {
    int id;
    int port;
    char address[40];
};


struct serverInfo serverList[10];

void *networking_thread(void *dest_addr);

playerNames *getPlayers();
void *get_in_addr(struct sockaddr *sa);
void sendJoinGame(int id);
void sendChatMsg(char *message, int msglen);
void readParams(char *full_address, char *host, char *port);
char *createFullAddress(char *host, char *port);

#endif /* SERVER_H_ */
