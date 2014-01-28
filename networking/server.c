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

// Structure for the "gamefield"
struct Field {
    char mat[6][7]; // The game matrix
    int placed; // How many are placed to know when full
};

// This creates gamefield and sets all field cells to 'x'
struct Field init_field(void){
    struct Field gamefield;

    int row, column;

    for(column=0;column<7;column++)
    {
        for(row=0;row<6;row++)
        {
            gamefield.mat[row][column] = 'x';
        }
    }
    return gamefield;
}

// Not my invention, got this from stackoverflow.com
// http://stackoverflow.com/questions/4636575/win-conditions-for-a-connect-4-like-game
// Adapted to C from C++
int is_linear_match(struct Field *gamefield, int x, int y, int stepx, int stepy)
{
	// Value of starting position
	char startvalue = gamefield->mat[x][y];
	if (startvalue == 'x') return 0;

	// Confirm if values after it match
	for (int i = 1; i < 4; i++){
		if (gamefield->mat[x+i * stepx][y + i * stepy] != startvalue)
			return 0;
	}
	// If we got here then there was a match
	return 1;
}

// These I made myself
// Check horizontal from all possible starting points
int is_horizontal_match(struct Field *gamefield)
{
	for(int i=0; i<6; i++)
	{
		for(int j=0; j<4; j++){
			if (is_linear_match(gamefield, i, j, 0, 1) == 1)
					return 1;
		}
	}
	return 0;
}
// Check vertical from all possible starting points
int is_vertical_match(struct Field *gamefield)
{
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<7; j++){
			if (is_linear_match(gamefield, i, j, 1, 0) == 1)
					return 1;
		}
	}
	return 0;
}
// Check diagonal down from all possible starting points
int is_diagonal_down_match(struct Field *gamefield)
{
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<4; j++){
			if (is_linear_match(gamefield, i, j, 1, 1) == 1)
					return 1;
		}
	}
	return 0;
}
// Check diagonal up from all possible starting points
int is_diagonal_up_match(struct Field *gamefield)
{
	for(int i=3; i<6; i++)
	{
		for(int j=0; j<4; j++){
			if (is_linear_match(gamefield, i, j, -1, 1) == 1)
					return 1;
		}
	}
	return 0;
}
// Win condition check
int win_condition_met(struct Field *gamefield)
{
	if (is_horizontal_match(gamefield) == 1) return 1;
	if (is_vertical_match(gamefield) == 1) return 1;
	if (is_diagonal_up_match(gamefield) == 1) return 1;
	if (is_diagonal_down_match(gamefield) == 1) return 1;
	return 0;
}

// Place game mark
int insert_to_column(int col, struct Field *gamefield, Player player){

	char marker;
	if (player.number == 0) marker = '0';
	else marker = '1';
    int i;
    int success = 0;
    if(col >= 7) return 0;
    for(i=5;i>=0;i--)
    {
        if (gamefield->mat[i][col] == 'x')
        {
            gamefield->mat[i][col] = marker;
            success = 1;
            break;
        }
    }
    return success;
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

	struct Field gamefield = init_field();

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
		}
		else if (msgtype == 2) // player ready
		{
			if(gamestate == 0){
				//set player's state to ready
				int pl = -1; // player
				for (int i = 0; i<numplayers; i++)
				{
					// Which player
					if(strcmp(ipstr, players[i].address) == 0 &&
							p_port == players[i].port){
						pl = i;
						break;
					}
				}
				if( pl != -1)
				{
					// Set status ready
					players[pl].status = 1;
					printf("player %d is ready\n", players[pl].number);
				}
				// next gamestate if all players are ready
				if(all_ready(players, numplayers) == 1){
					gamestate = 1;
					printf("Game start\n");
					send_start(players, sockfd, numplayers);
					//send the turn message to the first player
					send_turn(players[0], sockfd);

				}
			} else
			{
				; // send errormessage
			}

		}

		else if (msgtype == 5){ //player placed mark
			int pl = -1;
			for (int i = 0; i<numplayers; i++)
			{
				// Which player
				if(strcmp(ipstr, players[i].address) == 0 &&
						p_port == players[i].port){
					pl = i;
					break;
				}
			}
			// Check the right player turn
			if (gamestate == 1 && turn == pl){
				// Get column
				int column;
				column = unpackcolumn(inbuf);
				// Place it
				int success = insert_to_column(column, &gamefield, players[pl]);
				if(success == 1)
				{
					// Check win after placement
					if(win_condition_met(&gamefield) == 1){
						gamestate = 2;
						printf("Player %d won the game\n", turn);
						// Send area
						send_area(players, sockfd, numplayers, gamefield);
						// Send win
						send_win(players, sockfd, numplayers, turn);
					}
					else {
						// Send area and turn
						send_area(players, sockfd, numplayers, gamefield);
						turn++;
						if(turn == numplayers) turn = 0;
						send_turn(players[turn], sockfd);
					}
				}
			}
			//wrong player
			else{
				; //send errormessage
			}
		}// end messagetype
	} // end while

	close(sockfd); // close socket

	return 0;
}
