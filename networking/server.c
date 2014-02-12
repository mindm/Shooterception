/* Joonas Blomberg 0342152 <joonas.blomberg@lut.fi>
 * server.c
 *
 *
 *
 */
#include "server.h"
#include "packets.h"

// I/O buffers
char outbuf[MAXDATASIZE];
char inbuf[MAXDATASIZE];

// This handles IPv4 and IPv6 addresses dynamically
// Not my own invention, this is from Beej's tutorial (see readme)
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


// Sends ok message
void send_ok(Player player, int sock, int numplayers)
{
	//returns size
	int datasize = packok(outbuf, numplayers+1, player.number);

	//sendto because no connection
	sendto(sock, outbuf, datasize, 0,
				(struct sockaddr *)&player.their_addr, player.addr_size);
	outbuf[0] = '\0';
	printf("ok-packet sent, len %d\n", datasize);
}
// Sends turn message
void send_turn(Player player, int sock)
{
	int datasize = packturn(outbuf);

	sendto(sock, outbuf, datasize, 0,
					(struct sockaddr *)&player.their_addr, player.addr_size);

	outbuf[0] = '\0';
	printf("turn-packet sent, len %d\n", datasize);
}
// Sends start message to all players
void send_start(Player *players, int sock, int numplayers)
{
	int datasize = packstart(outbuf, 7, 6, numplayers);

	Player tmp;
	for(int i = 0; i < numplayers; i++)
	{
		tmp = players[i];
		sendto(sock, outbuf, datasize, 0,
							(struct sockaddr *)&tmp.their_addr, tmp.addr_size);
	}
	outbuf[0] = '\0';
}
// Sends area message to all players
void send_area(Player *players, int sock, int numplayers, struct Field gamefield)
{
	char *p_area;
	p_area = gamefield.mat[0];
	int datasize = packarea(outbuf, p_area, 6, 7);
	Player tmp;
	for(int i = 0; i < numplayers; i++)
	{
		tmp = players[i];
		sendto(sock, outbuf, datasize, 0,
							(struct sockaddr *)&tmp.their_addr, tmp.addr_size);
	}
	outbuf[0] = '\0';
	printf("area-packet sent, len %d\n", datasize);

}
// Sends winner message to all players
void send_win(Player *players, int sock, int numplayers, int winner)
{
	int datasize = packwinner(outbuf, winner);

	Player tmp;
	for(int i = 0; i < numplayers; i++)
	{
		tmp = players[i];
		sendto(sock, outbuf, datasize, 0,
							(struct sockaddr *)&tmp.their_addr, tmp.addr_size);
	}
	outbuf[0] = '\0';
}
// Would send error if it was implemented
/*
void send_error(Player player, int errno)
{
	char message[50];
	int datasize = packerror(outbuf, errno, message);
} */
// Checks if all players are ready
int all_ready(Player *players, int numplayers)
{
	int rdy = 1;
	for (int i = 0; i < numplayers; i++)
	{
		if (players[i].status == 0)
			rdy = 0;
	}
	return rdy;
}

int main(int argc, char *argv[])
{
	// Some variables for connection
	struct addrinfo hints, *res, *iter;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size; // address size
	int status, yes = 1;
	char ipstr[INET6_ADDRSTRLEN]; //Store ip-address
	int p_port; //store port
	int sockfd, numbytes;
	// 0 = looking for players, 1 = game running, 2 game has ended
	int gamestate = 0;
	// who's turn
	int turn = 0;


	Player players[MAX_PLAYERS];
	int numplayers = 0;

	//Hard coded arguments at this point
	char *port;

	// For parsing options
	extern char *optarg;
	extern int optopt;
	int optc;


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

	while(1){
		printf("server: waiting for packet...\n");

		// Save address sice and read with recvfrom
		sin_size = sizeof(their_addr);
		if ((numbytes = recvfrom(sockfd, inbuf, MAXDATASIZE-1, 0,
				(struct sockaddr *)&their_addr, &sin_size)) == -1) {
			perror("rcvfrom");
			exit(1);
		}

		uint16_t msgtype = getmessagetype(inbuf); // get messagetype

		// Address and port in readable form
		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),
				ipstr, sizeof(ipstr));
		p_port = ntohs(get_in_port((struct sockaddr *)&their_addr));
		printf("server: got packet from %s, %d\n",
				ipstr, p_port);
		printf("server: packet is %d bytes long\n", numbytes);
		inbuf[numbytes] = '\0';

		// Join
		if (msgtype == 0)
		{
			// check if players are full
			if (numplayers < MAX_PLAYERS && gamestate == 0){
			printf("Got a new player\n");
			// I guess there's a better way to do this but here goes nothing
			Player player;
			players[numplayers] = player;
			strcpy(players[numplayers].address, ipstr);
			players[numplayers].port = p_port;
			players[numplayers].number = numplayers;
			players[numplayers].status = 0; // 0 = connected
			players[numplayers].their_addr = their_addr;
			players[numplayers].addr_size = sin_size;

			// send ok after join
			send_ok(players[numplayers], sockfd, numplayers);
			numplayers += 1;

			}
			else // Full
			{
				; // send errormessage
			}
		}// end messagetype
		
		
	} // end while

	close(sockfd); // close socket

	return 0;
}
