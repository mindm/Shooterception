/* Joonas Blomberg 0342152 <joonas.blomberg@lut.fi>
 * client.c
 *
 *
 * readparams for connect
 */

#include "networking_client.h"

// These are going to be used globally
char outbuffer[MAXDATASIZE];
int lenout;

// This handles IPv4 and IPv6 addresses dynamically
// Not my own invention, this is from Beej's tutorial (see readme)
void *get_in_addr(struct sockaddr *sa)
{
	//IPv4
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    //IPv6
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


playerNames pl_names;

char player1[16];
char player2[16];
char player3[16];
char player4[16];

playerNames *getPlayers()
{
    return &pl_names;
}


void sendChatMsg(char *message, int msglen)
{
    lenout = packChatMessage(message, msglen);
}

void sendJoinGame(int id)
{
    int i;
    for (i = 0; i < 9; i++)
    {
        if (serverList[i].id)
        {
            if (serverList[i].id == id)
                break;
        }
    }
    
    
}

void setPlayerNames(player1, player2, player3, player4)
{
    pl_names.playerCount = 0;
    if (strlen(player1) > 0)
    {
        memcpy(pl_names.name[0][0], player1, 16);
        pl_names.playerCount = 1;
    }
    if (strlen(player2) > 0)
    {
        memcpy(pl_names.name[1][0], player2, 16);
        pl_names.playerCount = 2;
    }
    if (strlen(player3) > 0)
    {
        memcpy(pl_names.name[2][0], player3, 16);
        pl_names.playerCount = 3;
    }
    if (strlen(player4) > 0)
    {
        memcpy(pl_names.name[3][0], player4, 16);
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

void *networking_thread(void *dest_addr)
//int main() 
{

	// Some variables for connection
	struct addrinfo hints, *res, *iter;
	int status;
	char ipstr[INET6_ADDRSTRLEN];
	int sockfd, numbytes;
	char buf[MAXDATASIZE];

	char *host;
	char *port;

	// For parsing options
	extern char *optarg;
	extern int optopt;
	int optc;


	struct timeval tv;
	fd_set readfds;
	fd_set writefds;
	int fdmax; // biggest file descriptor

	//clear sets
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	//Player info
	//int playernum;

	//clear buffer
	memset(outbuffer,'\0', MAXDATASIZE);

	//set hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // Don't force IPv4 or v6
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family=PF_INET;

	// Check command line options
	/*
	while ((optc = getopt(argc,argv,":64h:p:")) != -1) {
		switch (optc) {
			case 'h':
				host = optarg;
				break;
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
	}*/

	// Are both given?
	if(!host || !port) {
		printf("Some parameter is missing\n");
		return -1;
	}

	if ((status = getaddrinfo(host, port, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return 1;
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
		return 1;
	}

	freeaddrinfo(res); // Addrinfo not needed anymore

	// Show in readable format
	inet_ntop(res->ai_family, get_in_addr((struct sockaddr *) res->ai_addr),
			ipstr, sizeof(ipstr));
	printf("client: connecting to %s\n", ipstr);

	// set biggest sock
	fdmax = sockfd;

	memset(&tv,0,sizeof(tv));
	tv.tv_usec = 0; //microseconds
	tv.tv_sec = 1; //seconds

	int running = 1;
	lenout = 0;


	int rval = 0;

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
				numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0);
				buf[numbytes] = '\0';
				uint8_t msgtype = getmessagetype(buf); //unpack messagetype

				// ok
				printf("%d\n", msgtype);
				
				
				if( msgtype == SERVERSTATE)
				{
				    //
				}
				
				else if (msgtype == LOBBYSTATE)
				{
				    unpackLobbyState(buf, player1, player2, player3, player4);
				    setPlayerNames(player1, player2, player3, player4);
				    
				}
				else if (msgtype == CHATRELAY)
				{   
				    int msglen;
					char* newMessage;
				    unpackChatRelay(buf, newMessage, &msglen);
				    //addLog(newMessage, msglen);
				}
				else if (msgtype == GAMESTART)
				{
				    int gameLevel;
				    unpackGameStart(buf, &gameLevel);
				}
				else if (msgtype == ERROR)
			    {
			        int errorCode;
			        //unpackError(buf, &errorCode);
			    }
				
			} // end read inputsocket
			// if something to output ->  send
			if(FD_ISSET(sockfd,&writefds)){
				numbytes = send(sockfd, outbuffer, lenout, 0);
				memset(outbuffer,'\0', MAXDATASIZE);
				lenout = 0;
			}

			// User gave some input
			if(FD_ISSET(fileno(stdin),&readfds)){

                //Input ??
			}

			// do UI things here
		}
		// timeout not needed
		//set new timevalues
		//tv.tv_usec = 10000; //seconds
		//ui_draw_console(cswin);
	}
	// free memory and close socket

	close(sockfd);

	return NULL;
}
