/*
* CT30A5002 Games and Networking 2013-14
* Authors:
* Arttu Hanska (0342181) - arttu.hanska@lut.fi
* Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
* Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "networking_server.h"
#include "packets.h"
#include "../gameLogic.h"

// I/O buffers
char outbuf[MAXDATASIZE];
char inbuf[MAXDATASIZE];
int lenout = 0;
int sendMask = 100; // Mask to determine clients receiving packets - 100 = all

// Get non-binary address, IPv4 or IPv6
char* getInAddr(struct sockaddr *sa, char* ipstr)
{
    printf("Server: Get address in non-binary form\n");

    if (sa->sa_family == AF_INET) {
        inet_ntop(AF_INET,&(((struct sockaddr_in*)sa)->sin_addr), ipstr, INET_ADDRSTRLEN);
        
    }
    else if(sa->sa_family == AF_INET6){
        inet_ntop(AF_INET6,&(((struct sockaddr_in6*)sa)->sin6_addr), ipstr, INET6_ADDRSTRLEN);
    }
    return ipstr;
}
// Get port, IPv4 or IPv6
int getInPort(struct sockaddr *sa, int p_port)
{
    printf("Server: Get port\n");

    if (sa->sa_family == AF_INET) {
        p_port = (((struct sockaddr_in*)sa)->sin_port);
        printf("IPv4 port %d\n",p_port);
    }
    else if (sa->sa_family == AF_INET) {
        p_port = (((struct sockaddr_in6*)sa)->sin6_port);
        printf("IPv6 port %d\n",p_port);
    }
    return p_port;
}

int main(int argc, char *argv[])
{
    printf("Server: Start main\n");

	// Some variables for connection
	struct addrinfo hints, *res, *iter;
	struct sockaddr_storage their_addr; // connector's address information
	
	player_n* tempAddress = (player_n*)malloc(sizeof(player_n));
    
	socklen_t sin_size; // address size
	game* gameState = NULL;

    if(gameState == NULL){ // If game doesn't exist, create it
        gameState = initGame();    
    }
    
    player* playerInfo = NULL; // Create player struct for parameter passing
    
    fd_set readfds; // File descriptor sets for select
    fd_set writefds;
    int fdmax; // Maximum file descriptor number
        
	char ipstr[INET6_ADDRSTRLEN]; //Store ip-address
	int p_port = 0; //store port
	int sockfd = 0;
	int size = 0;
	int sentbytes = 0;
	int recvbytes = 0;
    int status = 0;
    int yes = 0;
    int rval = 0;
    int messageNumber = 0;
    int gameLevel = 0;
    
	// For parsing options
	extern char *optarg;
	extern int optopt;
	int optc;
    
    char *port;

    // Temp values for joining players
    char gameName[16];
    int maxPlayers = 0;
    char playerName[16];
    
    
    // Varible to store chat messages
    char message[CHATMESSAGE_LENGTH];
    
    // Sets hints for binding socket
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
    

	while ((optc = getopt(argc,argv,":64p:")) != -1) {
		switch (optc) {
			case 'p':
				port = optarg;
				break;
			case '4':
				hints.ai_family=PF_INET; 	// IPv4
			case '6':
				hints.ai_family=PF_INET6; 	// IPv6
				break;
			case ':':
				printf("Parameter -%c is missing a operand\n", optopt);
				return -1;
			case '?':
				printf("Unknown parameter\n");
				break;
		}
	}
    
	if (!port)
	{
		printf("Port is missing, give -p <port>\n");
		return -1;
	}
    
	//getaddrinfo
	if ((status = getaddrinfo(NULL, port, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
	}
    
    // Go through addrinfo results
	for(iter = res; iter != NULL; iter = iter->ai_next) {
		//socket
		if ((sockfd = socket(iter->ai_family, iter->ai_socktype,
				iter->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
		// Set reuse address
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		//bind socket
		if (bind(sockfd, iter->ai_addr, iter->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}
    
    // No results
	if (res == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		return 1;
	}
    
    freeaddrinfo(res); // Done with addrinfo
    
    printf("Server: Enter main loop\n");
	
	while(1)
	{
	
	    printf("Server: Waiting for data\n");
	    
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
		// TODO: Add timeout
		if ((rval = select(fdmax+1,&readfds,&writefds,NULL, NULL)) > 0) {

			//listening socket got something
			if(FD_ISSET(sockfd,&readfds)){ 
			
			    printf("Server: Reveived data\n");
			    
			    sin_size = sizeof(their_addr);
				recvbytes = recvfrom(sockfd, inbuf, MAXDATASIZE-1, 0, (struct sockaddr *) &their_addr, &sin_size);
				inbuf[recvbytes] = '\0';
				uint8_t msgtype = getmessagetype(inbuf); //unpack messagetype
				
				// Get non-binary address
				getInAddr((struct sockaddr *) &their_addr,ipstr); 
                memcpy(tempAddress->address, ipstr, INET6_ADDRSTRLEN);
                
                // Get port
                getInPort((struct sockaddr *) &their_addr,p_port);
                tempAddress->port = p_port;
                
                printf("Server: Store port %d to struct\n",p_port);
                printf("Server: Value in struct: %d\n",tempAddress->port);
                                
                tempAddress->their_addr = their_addr;

                tempAddress->addr_size = sizeof(their_addr);		
                		
			    printf("Server: Got %d byte(s) from %s:%d\n",recvbytes,tempAddress->address,tempAddress->port);

                // Create game
                if(msgtype == 1)
                {
                    printf("Server: Received createGame message\n");
                    
                    unpackCreateGame(inbuf, gameName, &maxPlayers, playerName);

                    // Game state for new game
                    gameState = newGame(gameState, gameName, maxPlayers);  

                    // Change to inLobby state
                    gameState->currentState = 1;
                    
                    // Add first player to game
                    gameState = addPlayer(gameState, tempAddress, playerName);
                    
                    // Update lobby and send lobbyState
                    updateLobby(gameState, outbuf);       
                }
                // Join game
                else if(msgtype == 2)
                {
                    printf("Server: Received joinGame message\n");
                    
                    unpackJoinGame(inbuf, gameName, playerName);
                    
                    // Add player to game
                    gameState = addPlayer(gameState, tempAddress, playerName);
                    
                    // Update lobby and send lobbyState
                    updateLobby(gameState, outbuf);       
                }
                // Start game
                else if(msgtype == 3)
                {
                    printf("Server: Received startGame message\n");
                                    
                    unpackGameStart(inbuf, &gameLevel); // Game level not needed
                    startGame(gameState, outbuf, gameLevel);
                }
                // Client state
                else if(msgtype == 4)
                {
                    printf("Server: Received clientState message\n");
                                    
                    unpackClientState(inbuf, playerInfo, &messageNumber/*, int *timeReply*/);
                    
                    // Update player characters location, view direction and if the PC has shot
                    gameState = updatePlayerInfo(gameState, tempAddress, playerInfo->xcoord, playerInfo->ycoord, playerInfo->viewDirection, playerInfo->hasShot);     
                }
                // clientExit
                else if(msgtype == 5)
                {
                    printf("Server: Received clientExit message\n");      
                              
                    gameState = removePlayer(gameState, tempAddress);
                }
                // Chat message
                else if(msgtype == 6)
                {
                    printf("Server: Received chat message\n");                                
                    unpackChatMessage(inbuf, message);
                    relayChatMessage(gameState, outbuf, message);
                }              
                // Ack 
                else if(msgtype == 0)
                {
                    printf("Server: Received ack message\n");                                
                }                                                                                 
				
			} // end read inputsocket
			
			// if something to output ->  send
			if(FD_ISSET(sockfd,&writefds)){
			    if(sendMask == 100){
			        for(int i=0;i<=gameState->playerCount;i++){
				        sentbytes = sendto(sockfd, outbuf, lenout, 0, (struct sockaddr *) &(gameState->playerList[i].connectionInfo->their_addr), gameState->playerList[i].connectionInfo->addr_size);
			        }
				    memset(outbuf,'\0', MAXDATASIZE);
				    lenout = 0;
				} else{
				    sentbytes = sendto(sockfd, outbuf, lenout, 0, (struct sockaddr *) &(gameState->playerList[sendMask].connectionInfo->their_addr), gameState->playerList[sendMask].connectionInfo->addr_size);	
				    sendMask = 100; // Reset value  			
				    
				} 
			} 

			// Game Logic here
			gameState = checkSpawnTimer(gameState);
            // Spawn enemy if spawn rate allows and MAXENEMIES is not full
            if((gameState->enemySpawnRate = 0) && (gameState->enemyCount < MAXENEMIES)){
                // Spawn enemy to random border coordinate with level base speed and PC to follow
                gameState = addEnemy(gameState);
                gameState->enemyCount += 1;
            }
            
            // Update all enemy locations
            gameState = updateEnemyLocations(gameState);
            
            // Are player character and enemy colliding
            gameState = checkCollision(gameState);
            
            // Check end condition
            if(checkEnd(gameState) == 1){
                // Player's have won
                // Move to lobby and set next level parameters
            }
            else if(checkEnd(gameState) == 2){
                // All PCs dead, game lost
                // Shut down game, return to main menu
            }
            
            // Relay chat message to correct clients
            //relayChat();
            
            // Send game state to all clients
            // This sent twice?
            //sendGameState(gameState);
    
            gameState = resetEnemyHits(gameState);
            gameState = resetPlayerCollisions(gameState);

		}	
	} // End while loop
	
    // Cleanup after the game ends
    free(tempAddress);
    freeGame(gameState);

	return 0;
}


void setLenout(int size)
{
    lenout = size;
}

void setSendMask(int mask)
{
    sendMask = mask;
}
