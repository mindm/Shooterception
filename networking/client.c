/* Joonas Blomberg 0342152 <joonas.blomberg@lut.fi>
 * client.c
 *
 *
 *
 */

#include "client.h"
#include "packets.h"

// These are going to be used globally
char textbuffer[MESSAGEBUFFER] = {0};
char outbuffer[BUFFER];
int lenout;

// Message and command logging on screen
char msglog[LOGSIZE][MESSAGEBUFFER];
int logposition = 0;
int logentries = 0;

// Curses cursor location
int curx = 0;
int cury = 8;

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

// Structure for the "gamefield", not really necessary to be struct
struct Field {
    char mat[6][7];
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

// Draws the UI
void ui_draw_game(struct Field *gamefield)
{
	// screen has to be cleared first
	clear();
	printw("---------");
	int i,j;
    for(i=1;i<7;i++)
    {
        for(j=0;j<=8;j++)
        {
        	// Draws those "walls"
			if(j == 0 || j == 8)
				mvaddch(i, j, ACS_VLINE);
			else
				mvaddch(i, j, gamefield->mat[i-1][j-1]);
        }
    }
    printw("\n---------\n");
    // User input here
    printw("%s\n", textbuffer);
    printw("-----Log-----\n");
    // Handling messages (from Jussi's example code)
    for(int row = 0; row < LOGSIZE; row++) {
    	if(strlen(&msglog[row][0]) > 0) printw("%d: %s\n",row+logentries,&msglog[row][0]);

    }
    // Set cursor to where user input is
    move(cury, curx);
    refresh();

}

// Function that handles user input when user presses Enter
int handle_in(char* message, int msglen)
{
	// Scanning (parsing) user input
    char opt_1[MESSAGEBUFFER];
    int opt_2;
    int n;
    // Adds command to messages
    add_message(message, msglen);
    // How many arguments, 1 or 2
    n = sscanf(message, "%s %d", opt_1, &opt_2);

	// user input -> pack message -> send
    if (n == 1)
    {
        if(strcmp(opt_1, "/quit") == 0)
            return 1; //exit
        if(strcmp(opt_1, "/join") == 0)
            lenout = packjoin(outbuffer);
        if(strcmp(opt_1, "/ready") == 0)
            lenout = packready(outbuffer);
    } else if (n == 2){
    	if(strcmp(opt_1, "/place") == 0)
    		lenout = packcolumn(outbuffer, opt_2);
    }
    return 0; // return "don't wanna exit"
}


// Adds messages to the message list
void add_message(char* message, int msglen)
{
    if(logposition < LOGSIZE) {
		memset(&msglog[logposition][0],0,MESSAGEBUFFER);
		memcpy(&msglog[logposition][0],message,msglen); // Copy to the position
		logposition++; // Increase counter
    }
    else {
        // Move the entries
        for(int i = 0; i < LOGSIZE-1; i++) memcpy(&msglog[i][0],&msglog[i+1][0],MESSAGEBUFFER);
        memset(&msglog[LOGSIZE-1][0],0,MESSAGEBUFFER);
        memcpy(&msglog[LOGSIZE-1][0],message,msglen); // Replace the last with the new one
        logentries++; // Increase counter
      }
}

int main(int argc, char *argv[])
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
	int playernum;

	//clear buffer
	memset(outbuffer,'\0', BUFFER);

	//set hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // Don't force IPv4 or v6
	hints.ai_socktype = SOCK_DGRAM;

	// Check command line options
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
	}

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

	//NCURSES starts here
    initscr();
    struct Field gamefield = init_field();
    char *p_area;
    p_area = gamefield.mat[0];

    ui_draw_game(&gamefield);
    move(8, 0);
    refresh();

    //set these so that user input is possible
    cbreak();
    noecho();
    intrflush(stdscr, FALSE); // Prevent interrupt flush
    keypad(stdscr,TRUE); // Allow keypad usage

	// readchar and position
    int readc;
    int textpos = 0;

	// NCURSES OVER

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
				uint16_t msgtype = getmessagetype(buf); //unpack messagetype

				// ok
				if (msgtype == 1){
					int count;
					unpackok(buf, &count, &playernum);
					char smes[MESSAGEBUFFER];
					sprintf(smes, "Server: ok. PlayerID %d. Players: %d.", playernum, count);
					int ss = strlen(smes);
					add_message(smes, ss);
				} // start
				if (msgtype == 3){
					int cols, rows, players;
					unpackstart(buf, &cols, &rows, &players);
					char smes[MESSAGEBUFFER];
					sprintf(smes, "Server: start. Playercount %d.", players);
					int ss = strlen(smes);
					add_message(smes, ss);
					sprintf(smes, "Server: Game with %d cols and %d rows", cols, rows);
					ss = strlen(smes);
					add_message(smes, ss);

				} // turn
				if (msgtype == 4){
					char smes[MESSAGEBUFFER];
					sprintf(smes, "Server: It's your turn. Place marker.");
					int ss = strlen(smes);
					add_message(smes, ss);
				} // area
				if (msgtype == 6) {
					char smes[MESSAGEBUFFER];
					sprintf(smes, "Server: Area updated");
					int ss = strlen(smes);
					add_message(smes, ss);
					unpackarea(buf, p_area, 6, 7);
				} //winner
				if (msgtype == 7) {
					int winner;
					unpackwinner(buf, &winner);
					if (winner == playernum) // if you are winner
					{
						char smes[MESSAGEBUFFER];
						sprintf(smes, "You won the game! Press Esc to quit.");
						int ss = strlen(smes);
						add_message(smes, ss);
					} else { // you lost
						char smes[MESSAGEBUFFER];
						sprintf(smes, "You lost the game. Press Esc to quit.");
						int ss = strlen(smes);
						add_message(smes, ss);
					}
				}
			} // end read inputsocket
			// if something to output ->  send
			if(FD_ISSET(sockfd,&writefds)){
				numbytes = send(sockfd, outbuffer, lenout, 0);
				memset(outbuffer,'\0', BUFFER);
				lenout = 0;
			}

			// User gave some input
			if(FD_ISSET(fileno(stdin),&readfds)){

				readc = getch(); // Get each keypress
				switch(readc) {
				case 27: // Escape key
					running = 0;
					break;
				case KEY_ENTER:
				case '\n':
					if(strlen(textbuffer) > 0)
						if (handle_in(textbuffer,textpos) == 1)
							running = 0; // Exit
					textpos = 0;
					curx = 0; // cursor position
					memset(&textbuffer, 0, MESSAGEBUFFER);
					break;
				case KEY_BACKSPACE:
				case KEY_DC:
					if (textpos > 0){
						textpos--;
						textbuffer[textpos] = '\0';
						curx -= 1;
					}
					break;
				default:
					if(textpos < MESSAGEBUFFER-1){
					textbuffer[textpos] = readc;
					textpos++;
					curx += 1; //cursor moves forward
					}
					break;
				}
			}

			ui_draw_game(&gamefield); // redraw
		}
		// timeout not needed
		//set new timevalues
		//tv.tv_usec = 10000; //seconds
		//ui_draw_console(cswin);
	}
	// free memory and close socket
	endwin();
	close(sockfd);

	return 0;
}
