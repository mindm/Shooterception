/*
* CT30A5002 Games and Networking 2013-14
* Authors:
* Arttu Hanska (0342181) - arttu.hanska@lut.fi
* Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
* Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "test_client.h"

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

// These are going to be used globally
char outbuffer[MAXDATASIZE];
char inbuffer[MAXDATASIZE];
int lenout;
int lenin;

int main(int argc, char *argv[]){

  /*  if (argc != 2){
        printf("Usage: Give packet name as first argument\n\n");
        printf("Options are: chatMessage, createGame, joinGame, \nstartGame, clientState, clientExit, ack\n\n");
        return 0;
    } */

	// Some variables for connection
	struct addrinfo hints, *res, *iter;
	int status;
	char ipstr[INET6_ADDRSTRLEN];
	int sockfd, numbytes;

    // For testing
    int i = 0;
    int size = 0;
    char command[20];
    
    char *host = "0.0.0.0";
	char *port = "8000";
	
    player testPlayer[1];
    testPlayer->xcoord = 0; // X coordinate, upper left corner
    testPlayer->ycoord = 0; // Y coordinate, upper left corner
    testPlayer->viewDirection = 0; // Direction the PC is facing, 0-359 degrees
    testPlayer->health = 0; // Three Health points, third hit kills
    testPlayer->hasShot = 0; // Has the player shot after sending the last clientState
    testPlayer->shotCooldown = 0; // PCs can shoot 2/s
    testPlayer->isHost = 0; // Is the player the host - 0: False, 1: True
    testPlayer->playerNumber = 0; // Player's number
    testPlayer->playerName = "testplayer0"; // Player's name
    testPlayer->connectionInfo = NULL; // pointer to struct containing sockaddr_storage
    testPlayer->isColliding = 0; // Is the PC colliding with enemy?

	// For parsing options
	//extern char *optarg;
	//extern int optopt;
	//int optc;

	fd_set readfds;
	fd_set writefds;
	int fdmax; // biggest file descriptor

	//clear sets
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	//clear buffer
	memset(outbuffer,'\0', MAXDATASIZE);

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
		fprintf(stderr, "client: failed to find connection");
		//return 1;
	}

	freeaddrinfo(res); // Addrinfo not needed anymore

	// Show in readable format
	inet_ntop(res->ai_family, get_in_addr((struct sockaddr *) res->ai_addr),
			ipstr, sizeof(ipstr));
	printf("Client: connecting to %s\n", ipstr);

	// set biggest sock
	fdmax = sockfd;
	int running = 1;
	lenout = 0;
	int rval = 0;

	printf("Client: Enter Main loop\n");
	
	while(running) {
	
	    printf("\nClient: Give message name to send\n");
	    
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
		
            scanf("%s",command);
	    
			//listening socket got something
			if(FD_ISSET(sockfd,&readfds)){
				numbytes = recv(sockfd, inbuffer, MAXDATASIZE-1, 0);
				inbuffer[numbytes] = '\0';
				uint8_t msgtype = getmessagetype(inbuffer); //unpack messagetype

				// ok
				printf("%d\n", msgtype);
				
			} // end read inputsocket
			// if something to output ->  send
			if(FD_ISSET(sockfd,&writefds)){
			    printf("Client: Send something\n");
				numbytes = send(sockfd, outbuffer, lenout, 0);
				memset(outbuffer,'\0', MAXDATASIZE);
				lenout = 0;
			}
		}
			
	    // Pack message and set it to outbuffer
	    if(strcmp(command,"chatMessage") == 0){
	        printf("Client: Sent chatMessage message\n");
	        
	        //int packChatMessage(char *buf, char *message)  		    
	        size = packChatMessage(outbuffer, "test message");
	        
	        // Set lenout to send message
            setLenout(size);
        } 
	    else if(strcmp(command,"createGame") == 0){
	        printf("Client: Sent createGame message\n");
	        
	        //int packCreateGame(char *buf, char *gameName, int maxPlayers, char *playerName)
	        size = packCreateGame(outbuffer, "testgame", 4, "testplayer1");
	        
	        // Set lenout to send message
            setLenout(size);
        }                 
	    else if(strcmp(command,"joinGame") == 0){
	        printf("Client: Sent joinGame message\n");
	        
	        //int packJoinGame(char *buf, char *gameName, char *playerName)
		    size = packJoinGame(outbuffer, "testgame", "testplayer2");
	                
	        // Set lenout to send message
            setLenout(size);
        }    
	    else if(strcmp(command,"startGame") == 0){
	        printf("Client: Sent startGame message\n");
	        
	        // int packStartGame(char *buf)
	        size = packStartGame(outbuffer);
	        
	        // Set lenout to send message
            setLenout(size);
        }    
	    else if(strcmp(command,"clientState") == 0){
	        printf("Client: Sent clientState message\n");
	        
	        //int packClientState(char *buf, player playerInfo, int messageNumber/*, int timeReply*/)
	        size = packClientState(outbuffer, testPlayer, 1);
	        
	        // Set lenout to send message
            setLenout(size);
        } 
	    else if(strcmp(command,"clientExit") == 0){
	        printf("Client: Sent clientExit message\n");
	        
	        size = packClientExit(outbuffer);
	        
	        // Set lenout to send message
            setLenout(size);
        }          
	    else if(strcmp(command,"ack") == 0){
	        printf("Client: Sent ack message\n");
	        
	        size = packAck(outbuffer);
	        
	        // Set lenout to send message
            setLenout(size);
        }	
        else{
            printf("Now valid packet name\n");
        }                   
	}
	// free memory and close socket

	close(sockfd);
	return 0;
}

void setLenout(int size)
{
    lenout = size;
}
