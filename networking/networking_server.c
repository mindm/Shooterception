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

// This handles IPv4 and IPv6 addresses dynamically
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
// Same for port
int get_in_port(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return (((struct sockaddr_in*)sa)->sin_port);
    }

    return (((struct sockaddr_in6*)sa)->sin6_port);
}



int main(int argc, char *argv[])
{
	// Some variables for connection
	struct addrinfo hints, *res, *iter;
	struct sockaddr_storage their_addr; // connector's address information
	player_n tempAddress[0]; 
	socklen_t sin_size; // address size
	
    game* gameState = NULL; // Create game struct
    
	char ipstr[INET6_ADDRSTRLEN]; //Store ip-address
	int p_port; //store port
	int sockfd, numbytes;
    int status, yes;

    
	// For parsing options
	extern char *optarg;
	extern int optopt;
	int optc;
    
    char *port;

    // Temp values for joining players
    char gameName[16];
    int maxPlayers = 0;
    char playerName[16];
    
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
		printf("Some parameter is missing\n");
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

	while(running)
	{
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
		if ((rval = select(fdmax+1,&readfds,&writefds,NULL, NULL)) > 0) {

			//listening socket got something
			if(FD_ISSET(sockfd,&readfds)){
			    sin_size = sizeof(their_addr);
				numbytes = recvfrom(sockfd, buf, MAXDATASIZE-1, 0, (struct sockaddr *) &their_addr, &sin_size);
				buf[numbytes] = '\0';
				uint8_t msgtype = getmessagetype(buf); //unpack messagetype
                                
                tempAddress[0].address = get_in_addr(their_addr);
                tempAddress[0].port = get_in_port(their_addr);
                tempAddress[0].their_addr = their_addr;
                tempAddress[0].addr_size = sizeof(their_addr);

                // Create game
                if(msgtype == 1)
                {
                    unpackCreateGame(inbuf, gameName, &maxPlayers, playerName);

                    // Game state for new game
                    gameState = newGame(gameName, maxPlayers);  

                    // Change to inLobby state
                    gameState->currentState = 1;
                    
                    // Add player to game
                    //addPlayer(game* gameState, player_n, char playerName[16]);
                    gameState = addPlayer(gameState, tempAddress[0], playerName);
                    
                    // Send lobbyState - only one player at the moment
                    int size = packLobbyState(outbuf, char *player1, "", "", "")  
                    setLenout(size);         
                }
                // Join game
                else if(msgtype == 2)
                {
                    unpackJoinGame(inbuf, gameName, playerName);
                    
                    // Add player to game
                    //addPlayer(game* gameState, int playerNumber, char playerName[16]);
                    gameState = addPlayer(gameState, tempAddress, playerName);
                    //updateLobby(gameState, outbuf);
                
                }
                // Start game
                else if(msgtype == 3)
                {
                    Startgame(gameState, outbuf);
                    
                }
                // Client state
                else if(msgtype == 4)
                {
                    
                }
                // clientExit
                else if(msgtype == 5)
                {
                
                }
                // Chat message
                else if(msgtype == 6)
                {
                
                }              
                // Ack 
                else if(msgtype == 0)
                {
                
                }                                                                                 
				// ok
				printf("%d\n", msgtype);
				
			} // end read inputsocket
			// if something to output ->  send
			if(FD_ISSET(sockfd,&writefds)){
			    if(sendMask == 100){
			        for(int i=0;i<=gameLogic->playerCount;i++){
				        numbytes = sendto(sockfd, outbuf, lenout, 0, (struct sockaddr *) &(gameLogic->playerList[i].connectionInfo.their_addr), &(gameLogic->playerList[i].connectionInfo.addr_size);
			        }
				    memset(outbuffer,'\0', MAXDATASIZE);
				    lenout = 0;
				} else{
				    numbytes = sendto(sockfd, outbuf, lenout, 0, (struct sockaddr *) &(gameLogic->playerList[sendMask].connectionInfo.their_addr), &(gameLogic->playerList[sendMask].connectionInfo.addr_size);	
				    sendMask = 100; // Reset value  			
				    
				} 
			}

			// User gave some input
			if(FD_ISSET(fileno(stdin),&readfds)){

                //Input ??
			}

			// do game logic things here

		}
		// timeout not needed
		//set new timevalues
		//tv.tv_usec = 10000; //seconds
		//ui_draw_console(cswin);
	}
    
/*    while(1)
    {
        //Server loop
        printf("server: waiting for packet...\n");
        
		// Save address size and read with recvfrom
		sin_size = sizeof(their_addr);
		if ((numbytes = recvfrom(sockfd, inbuf, MAXDATASIZE-1, 0,
				(struct sockaddr *)&their_addr, &sin_size)) == -1) {
			perror("rcvfrom");
			exit(1);
		}
        
        int msgtype = getmessagetype(inbuf); // get messagetype
        
        printf("msgtype: %d\n", msgtype);
    
    }
*/
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
