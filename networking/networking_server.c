/*
* CT30A5002 Games and Networking 2013-14
* Authors:
* Arttu Hanska (0342181) - arttu.hanska@lut.fi
* Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
* Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "networking_server.h"
#include "packets.h"

// I/O buffers
char outbuf[MAXDATASIZE];
char inbuf[MAXDATASIZE];

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
	socklen_t sin_size; // address size

	char ipstr[INET6_ADDRSTRLEN]; //Store ip-address
	int p_port; //store port
	int sockfd, numbytes;
    int status, yes;

	// For parsing options
	extern char *optarg;
	extern int optopt;
	int optc;
    
    char *port;
    
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
    
    while(1)
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