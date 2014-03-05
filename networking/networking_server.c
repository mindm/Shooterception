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
int sockfd = 0;

// Get non-binary address, IPv4 or IPv6
char* getInAddr(struct sockaddr *sa, char* ipstr)
{
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
    if (sa->sa_family == AF_INET) {
        p_port = (((struct sockaddr_in*)sa)->sin_port);
    }
    else if (sa->sa_family == AF_INET) {
        p_port = (((struct sockaddr_in6*)sa)->sin6_port);
    }
    return p_port;
}


void sendToMaster(game gameState)
{
    lenout = packUpdateState(outbuf, &gameState);
    setSendMask(200);
}

int main(int argc, char *argv[])
{
    printf("\nServer: Start main\n");

	// Some variables for connection
	struct addrinfo hints, *res, *iter;
	struct sockaddr_storage their_addr; // connector's address information

    // Master server address
    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=inet_addr("157.24.55.212");
    servaddr.sin_port=htons(6001);

	struct timeval tv;
    //struct timespec tv;
    
	player_n* tempAddress = (player_n*)malloc(sizeof(player_n));
    
	socklen_t sin_size; // address size
	game* gameState = initGame();
	
    printf("Server: initGame complete\n");
    
    player* playerInfo = (player*)malloc(sizeof(player)); // Create player struct for parameter passing
    
    fd_set readfds; // File descriptor sets for select
    fd_set writefds;
    int fdmax; // Maximum file descriptor number
        
	char ipstr[INET6_ADDRSTRLEN]; //Store ip-address
	int p_port = 0; //store port
	
	int recvbytes = 0;
    int status = 0;
    int yes = 0;
    int rval = 0;
    int msgNumber = 0;
    int gameLevel = 0;
    
	// For parsing options
	extern char *optarg;
	extern int optopt;
	int optc;
    
    char *port = NULL;

    // Temp values for joining players
    char gameName[GAMENAME_LENGTH];
    int maxPlayers = 0;
    char playerName[PLAYERNAME_LENGTH];
    
    
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
	    //timeval
        if(gameState->currentState == 2){
	        tv.tv_sec = 0;
            tv.tv_usec = 50000; // 50ms (= 50000 microseconds)
        } else {
            tv.tv_sec = 9001;
            tv.tv_usec = 0; 
        }
        
        //timespec
        //tv.tv_nsec = 50000; // 50ms (= 50000 microseconds = 50000000 nanoseconds)
                
	    printf("\nServer: Waiting for data\n\n");
	    
	    // Clear the sets of file descriptors
		FD_ZERO(&readfds); 
		FD_ZERO(&writefds);
		
		// Socket to the server
		FD_SET(sockfd, &readfds);
		if(sockfd > fdmax) fdmax = sockfd;
		
		// WRITESOCKET
		if (lenout > 0){
			FD_SET(sockfd, &writefds);
		}

		// Block until input (no timeval)
        
        // TODO: Start timeout when currentState == 2
		if ((rval = select(fdmax+1,&readfds,&writefds,NULL, &tv)) > 0) {
		//if ((rval = select(fdmax+1,&readfds,&writefds,NULL, NULL)) > 0) {
			//listening socket got something
			if(FD_ISSET(sockfd,&readfds)){  
			    
			    sin_size = sizeof(their_addr);
			    			    
				recvbytes = recvfrom(sockfd, inbuf, MAXDATASIZE-1, 0, (struct sockaddr *) &their_addr, &sin_size);
				
				inbuf[recvbytes] = '\0';
				uint8_t msgtype = getmessagetype(inbuf); //unpack messagetype
				
				// Get non-binary address
				getInAddr((struct sockaddr *) &their_addr,ipstr); 
                memcpy(tempAddress->address, ipstr, INET6_ADDRSTRLEN);
                
                // Get port
                p_port = getInPort((struct sockaddr *) &their_addr,p_port);
                tempAddress->port = p_port;
         
                tempAddress->their_addr = their_addr;

                tempAddress->addr_size = sizeof(their_addr);		
                		
			    printf("Server: Got %d byte(s) from %s:%d\n",recvbytes,tempAddress->address,tempAddress->port);

                // Create game - available in waitingGame state
                if(msgtype == 1 && gameState->currentState == 0)
                {
                    printf("Server: Received createGame message\n");
                    
                    unpackCreateGame(inbuf, gameName, &maxPlayers, playerName);

                    // Game state for new game
                    gameState = newGame(gameState, gameName, maxPlayers);  
                    printf("Server: newGame complete\n");

                    // Add first player to game
                    gameState = addPlayer(gameState, tempAddress, playerName);
                    printf("Server: addPlayer complete\n");
                    
                    // Update lobby and send lobbyState
                    updateLobby(gameState, outbuf);  
                    printf("Server: updateLobby complete\n");
                }
                // Join game - available in inLobby state
                else if(msgtype == 2 && gameState->currentState == 1)
                {
                    printf("Server: Received joinGame message\n");
                    
                    unpackJoinGame(inbuf, gameName, playerName);
                    
                    // Add player to game
                    gameState = addPlayer(gameState, tempAddress, playerName);
                    printf("Server: addPlayer complete\n");
                    
                    // Update lobby and send lobbyState
                    updateLobby(gameState, outbuf); 
                    printf("Server: updateLobby complete\n");                       
                }
                // Start game
                else if(msgtype == 3 && gameState->currentState == 1)
                {
                    printf("Server: Received startGame message\n");
                                    
                    unpackGameStart(inbuf, &gameLevel);

                    startGame(gameState, outbuf, gameLevel);
                    printf("Server: startGame complete\n");
                }
                // Client state
                else if(msgtype == 4)
                {
                    printf("Server: Received clientState message\n");
                                    
                    unpackClientState(inbuf, playerInfo, &msgNumber/*, int *timeReply*/);
                    
                    // Update player characters location, view direction and if the PC has shot
                    gameState = updatePlayerInfo(gameState, tempAddress, playerInfo->xcoord, playerInfo->ycoord, playerInfo->viewDirection, playerInfo->hasShot);     
                    printf("Server: updatePlayerInfo complete\n");       
                }
                // clientExit
                else if(msgtype == 5)
                {
                    printf("Server: Received clientExit message\n");      
                              
                    gameState = removePlayer(gameState, tempAddress);
                    printf("Server: removePlayer complete\n");                    
                }
                // Chat message
                else if(msgtype == 6)
                {
                    printf("Server: Received chat message\n"); 
                                                   
                    unpackChatMessage(inbuf, message);
                    
                    relayChatMessage(gameState, outbuf, message);
                    printf("Server: relayChatMessage complete\n");                    
                }              
                // Ack 
                else if(msgtype == 0)
                {
                    printf("Server: Received ack message\n");                                
                }   
                else{
                    printf("Unknown message type: %d\n",msgtype);
                }    
			} // end read inputsocket
			
			// if something to output ->  send
			if(FD_ISSET(sockfd,&writefds)){
			    		    
			    // Send to all players		    
			    if(sendMask == 100){
	    
			        for(int i=0;i<gameState->playerCount;i++){
			            sendto(sockfd, outbuf, lenout, 0, (struct sockaddr *) &(gameState->playerList[i].connectionInfo.their_addr), gameState->playerList[i].connectionInfo.addr_size);
			        }
			        		
			        printf("Sent packet to all players (%d)\n",gameState->playerCount);
			        		        
				    memset(outbuf,'\0', MAXDATASIZE);
				    lenout = 0;
				// Send to master server
				} else if (sendMask == 200) {
                    sendMask = 100;
                    sendto(sockfd, outbuf, lenout, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
                }
                // Send to one player
                else{
				    sendto(sockfd, outbuf, lenout, 0, (struct sockaddr *) &(gameState->playerList[sendMask].connectionInfo.their_addr), gameState->playerList[sendMask].connectionInfo.addr_size);	
				    sendMask = 100; // Reset value  	
				    		
		            printf("Sent packet to player %d\n",gameState->playerList[sendMask].playerNumber); 
				} 
			} 
		} // End select
		
        printf("currentState: %d\n",gameState->currentState);

	    // Game Logic here
	
	    //printf("\n***update functions start***\n");

        // currentState in inGame
	    if(gameState->currentState == 2){

	        for(int i=0;i<gameState->playerCount;i++){
	            printf("\nPlayer %d \nName: %s\nConnection: %s:%d\nxcoord:%d\nycoord:%d\n",gameState->playerList[i].playerNumber, gameState->playerList[i].playerName, gameState->playerList[i].connectionInfo.address,gameState->playerList[i].connectionInfo.port, gameState->playerList[i].xcoord, gameState->playerList[i].ycoord);
	        }
	        
            printf("gameLevel: %d\nenemyLimit: %d\nenemySpawnRate: %llu\n",gameState->levelNumber,gameState->enemyLimit, gameState->enemySpawnRate);

	        gameState = checkSpawnTimer(gameState);

            // Spawn enemy if spawn rate allows, MAXENEMIES not full, level limit not reached
            if((gameState->canSpawn == 1) && (gameState->enemyCount < MAXENEMIES) && (gameState->enemyCount<gameState->enemyLimit) && (gameState->deadEnemyCount != gameState->enemyLimit)){
                // Spawn enemy to random border coordinate
                gameState = addEnemy(gameState);
            }
            
            for(int i=0;i<gameState->enemyCount;i++){
                printf("Enemy %d, xcoord: %d, ycoord: %d, health: %d, following: %d\n",i, gameState->enemyList[i].xcoord,gameState->enemyList[i].ycoord,gameState->enemyList[i].health, gameState->enemyList[i].following);
            }

            // Update all enemy locations
            gameState = updateEnemyLocations(gameState);
            
            // Are player character and enemy colliding
            gameState = checkCollision(gameState);
            
            // Check end condition
            if(checkEnd(gameState) == 1){
                // Player's have won
                
                // Move to lobby and set next level parameters
                gameState = nextLevel(gameState,outbuf);
                
            }
            else if(checkEnd(gameState) == 2){
                // All PCs dead, game lost
                
                // Move to lobby and set first level parameters
                gameState = resetGame(gameState,outbuf);
            }
            else {
            	// Send game state to all clients
            	sendGameState(gameState, outbuf);
			}      
			      
            printf("Packet number: %d\n", gameState->msgNumber);
            printf("Enemy count: %d\n", gameState->enemyCount);            
            printf("Dead enemy count: %d\n", gameState->deadEnemyCount);            

            gameState = resetEnemyHits(gameState);
            gameState = resetPlayerCollisions(gameState);
            
	        //printf("***update functions end***\n");
        }
	} // End while loop
	
    // Cleanup after the game ends
    free(playerInfo);
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
