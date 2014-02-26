/* Joonas Blomberg 0342152 <joonas.blomberg@lut.fi>
 * client.c
 *
 *
 * readparams for connect
 */

#include "networking_client.h"

// These are going to be used globally
int lenout = 0;
int starting = 0;

// This handles IPv4 and IPv6 addresses dynamically
// Not my own invention, this is from Beej's tutorial (see readme)
void *get_in_addr(struct sockaddr *sa) {
	//IPv4
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    //IPv6
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

playerNames *getPlayers() {

    return &pl_names;
}

void sendChatMsg(char* message) {
    lenout = packChatMessage(outbuf, message);
}

void sendJoinGame(int id) {
	printf("Send joinGame\n");
	lenout = packJoinGame(outbuf, cl_gamesList.servers[id].gameName, pl_name_str);

}

void sendCreateGame(void) {
    printf("Send createGame\n");
	lenout = packCreateGame(outbuf, g_name_str, pl_num_max, pl_name_str);
}

void sendGameStart(void) {
	printf("Send startGame\n");
	lenout = packStartGame(outbuf);
}

void sendGamesQuery(void) {
	printf("Send Games query\n");
	lenout = packGamesQuery(outbuf);
}

void sendServerQuery(void) {
	printf("Send server query\n");
	lenout = packServerQuery(outbuf);
}

int getMaxPlayers(void) {
	return gameState->maxPlayers;
}

int getGameList(void) {

	int i;
	for(i = 0; i < cl_gamesList.count; i++) {
		strncpy(server_list[i]->name, cl_gamesList.servers[i].gameName, 17);
		server_list[i]->pl_num = cl_gamesList.servers[i].playerNumber;
		server_list[i]->pl_num_max = cl_gamesList.servers[i].maxPlayers;
	}

	return cl_gamesList.count;
}

int getServerList(void) { 

	int i;
	for(i = 0; i < cl_serverList.count; i++) {
		strncpy(server_list[i]->name, cl_serverList.servers[i].host, 46);
	}

	return cl_serverList.count;
}

int getGameStart(void) { 

	return starting;
}

void setPlayerNames(char* pl1, char* pl2, char* pl3, char* pl4) {
	
	pl_names.playerCount = 0;
    if (strlen(pl1) > 0) {
        strncpy(pl_names.name[0], pl1, 16);
        pl_names.playerCount = 1;
    }
    
    if (strlen(pl2) > 0) {
       strncpy(pl_names.name[1], pl2, 16);
        pl_names.playerCount = 2;
    }

    if (strlen(pl3) > 0) {
        strncpy(pl_names.name[2], pl3, 16);
        pl_names.playerCount = 3;
    }
    if (strlen(pl4) > 0) {
        strncpy(pl_names.name[3], pl4, 16);
        pl_names.playerCount = 4;
    }
}

//Full_address length should be 46+17 = 63 bits
//Copies full address into host and port
void readParams(char *full_address, char *host, char *port)
{
    memcpy(host, full_address, 46);
    memcpy(port, full_address+46, 17);
}

char *createFullAddress(char *host, char *port)
{       
    char *full_address = malloc(63);
    
    memcpy(full_address, host, 46);
    memcpy(full_address+46, port, 17);
    
    return full_address;
}

void sendClientState(struct SDLplayer *_player, int msgnum) {
	
	player pl_info;
 	pl_info.xcoord = _player->b.x;
	pl_info.ycoord = _player->b.y;
	pl_info.hasShot = _player->shooting;
	pl_info.viewDirection = _player->dir;

	lenout = packClientState(outbuf, pl_info, msgnum);

}

void sendClientExit() {
	lenout = packClientExit(outbuf);
}

int connectServer(int id, int joining) {

	// Some variables for connection
	struct addrinfo hints, *res, *iter;
	int status;

	//set hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // Don't force IPv4 or v6
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family=PF_INET;

	if(!joining) {
		if ((status = getaddrinfo(cl_serverList.servers[id].host, cl_serverList.servers[id].port, &hints, &res)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
			return 1;
		}
	}
	else {
		if ((status = getaddrinfo(cl_gamesList.servers[id].host, cl_gamesList.servers[id].port, &hints, &res)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
			return 1;
		}
	}

	// Iterate through results and use first usable one
	for(iter = res; iter != NULL; iter = iter->ai_next) {
		if ((sockfd = socket(iter->ai_family, iter->ai_socktype,
				iter->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}
		// Connect to use send and rcv
		if (connect(sockfd, iter->ai_addr, iter->ai_addrlen) == -1) {
			close(sockfd); // Sock has to be closed because connection failed
			perror("client: connect");
			continue;
		}

		break;
	}

	freeaddrinfo(res);
	return 0;
}

void updateEnemyStates(struct SDLenemy* _enemy, int i) {

    _enemy->health = gameState->enemyList[i].health;

	if(_enemy->health <= 0) 
		return;

    _enemy->b.x = gameState->enemyList[i].xcoord;
    _enemy->b.y = gameState->enemyList[i].ycoord;
    _enemy->dir = gameState->enemyList[i].viewDirection;
    _enemy->is_shot = gameState->enemyList[i].isShot;

}

void updatePlayerStates(struct SDLplayer* _player, int i) {

	_player->health = gameState->playerList[i].health;

	if(_player->health <= 0) 
		return;

	_player->b.x = gameState->playerList[i].xcoord;
	_player->b.y = gameState->playerList[i].ycoord;
	_player->dir = gameState->playerList[i].viewDirection;	
	_player->shooting = gameState->playerList[i].hasShot;
	_player->can_shoot = gameState->playerList[i].canShoot;
	_player->is_attacked = gameState->playerList[i].isColliding;

}

void *networking_thread(void *dest_addr)
{
    struct timespec tv;
    
	gameState = malloc(sizeof(game));

	// Some variables for connection
	struct addrinfo hints, *res, *iter;
	int status;
	char ipstr[INET6_ADDRSTRLEN];
	int numbytes;

	char *host = "0.0.0.0";
	char *port = "6001";

	// For parsing options
	extern char *optarg;
	extern int optopt;
	int optc;

	fd_set readfds;
	fd_set writefds;
	int fdmax; // biggest file descriptor

	//clear sets
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	//set hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // Don't force IPv4 or v6
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family=PF_INET;

	// Are both given?
	if(!host || !port) {
		printf("Some parameter is missing\n");
		//return -1;
	}

	if ((status = getaddrinfo(host, port, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		//return 1;
	}

	// Iterate through results and use first usable one
	for(iter = res; iter != NULL; iter = iter->ai_next) {
		if ((sockfd = socket(iter->ai_family, iter->ai_socktype,
				iter->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}
		// Connect to use send and rcv
		if (connect(sockfd, iter->ai_addr, iter->ai_addrlen) == -1) {
			close(sockfd); // Sock has to be closed because connection failed
			perror("client: connect");
			continue;
		}

		break;
	}
	// not found
	if (res == NULL) {
		printf("client: failed to find connection\n");
		//return 1;
		//exit(1);
	}

	freeaddrinfo(res); // Addrinfo not needed anymore

	// Show in readable format
	inet_ntop(res->ai_family, get_in_addr((struct sockaddr *) res->ai_addr),
			ipstr, sizeof(ipstr));
	printf("client: connecting to %s\n", ipstr);

	// set biggest sock
	fdmax = sockfd;

	int running = 1;
	lenout = 0;

	int rval = 0;

	printf("client loop\n");
	while(running) {
	
		tv.tv_sec = 1;
        tv.tv_nsec = 0;
        
		FD_ZERO(&readfds); // Clear the sets of file descriptors
		FD_ZERO(&writefds);
		// Socket to the server
		FD_SET(sockfd, &readfds);
		if(sockfd > fdmax) fdmax = sockfd;

		// STDIN
		FD_SET(fileno(stdin),&readfds);
		if(fileno(stdin) > fdmax) fdmax = fileno(stdin);

		// WRITESOCKET
		if (lenout > 0){
			FD_SET(sockfd, &writefds);
		}
		// Block until input (no timeval)
		if ((rval = select(fdmax+1,&readfds,&writefds,NULL, &tv)) > 0) {

			//listening socket got something
			if(FD_ISSET(sockfd,&readfds)){
				numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0);
				buf[numbytes] = '\0';
				uint8_t msgtype = getmessagetype(buf); //unpack messagetype

				// ok
				printf("%d\n", msgtype);		
				
				if( msgtype == SERVERSTATE) {
					int msgnum; printf("server state\n");
				    unpackServerState(buf, gameState, &msgnum);
					starting = 1;
				}
				
				else if (msgtype == LOBBYSTATE) {
				    unpackLobbyState(buf, pl1, pl2, pl3, pl4);
				    setPlayerNames(pl1, pl2, pl3, pl4);			    
				}

				else if (msgtype == CHATRELAY) {   
				    int msglen;
					char newMessage[100];
				    unpackChatRelay(buf, newMessage, &msglen);
				    addLog(newMessage, msglen);
				}

				else if (msgtype == GAMESTART) {
				    //gameLevel declared in SDLmain
				    unpackGameStart(buf, &gameLevel);
					
				}

				else if (msgtype == GAMELIST) {
					unpackGameList(buf, &cl_gamesList);
				}

				else if (msgtype == SERVERLIST) {
					unpackServerList(buf, &cl_serverList);
				}

				else if (msgtype == ERROR) {
			        int errorCode;
			        unpackError(buf, &errorCode);
			    }
				
			} // end read inputsocket
			// if something to output ->  send
			if(FD_ISSET(sockfd,&writefds)){
			    printf("Send something\n");
				numbytes = send(sockfd, outbuf, lenout, 0);
				memset(outbuf,'\0', MAXDATASIZE);
				lenout = 0;
			}

			// User gave some input
			//if(FD_ISSET(fileno(stdin), &readfds)) //Input ??

		}
	}
	// free memory and close socket

	close(sockfd);
	free(gameState);

	return NULL;
}
