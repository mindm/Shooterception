/*
* CT30A5002 Games and Networking 2013-14
* Authors:
* Arttu Hanska (0342181) - arttu.hanska@lut.fi
* Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
* Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#ifndef SERVER_H_
#define SERVER_H_

#define MAXDATASIZE 512
#define MAX_PLAYERS 4

#include "../generic.h"

// The playerinfo struct
typedef struct player_n
{
	int number;
	char address[INET6_ADDRSTRLEN];
	int port;
	struct sockaddr_storage their_addr;
	int addr_size;
    
} player_n;

void *get_in_addr(struct sockaddr *sa);
int get_in_port(struct sockaddr *sa);


#endif /* SERVER_H_ */
