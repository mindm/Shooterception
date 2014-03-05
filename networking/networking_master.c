/*
* CT30A5002 Games and Networking 2013-14
* Authors:
* Arttu Hanska (0342181) - arttu.hanska@lut.fi
* Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
* Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "networking_master.h"

// I/O buffers
char outbuf[MAXDATASIZE];
char inbuf[MAXDATASIZE];
int lenout = 0;

serverList server_list;

void getPort(char *port)
{
    FILE *fp;
    fp = fopen("master.cfg", "r");
    if (fp)
    {   
        fscanf(fp, "%s", port);
    }
    else 
    {
        strcpy(port, "6001");
        fp = fopen("master.cfg", "w");
        fprintf(fp, "%s\n", port);           
    }
    fclose(fp);
}

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

void printServer(server _server)
{
    printf("Server address: %s:%s\n", _server.host, _server.port);
    printf("State: %d Players: %d/%d\n", _server.serverState, _server.playerNumber, _server.maxPlayers);
    printf("Game name: %s\n\n", _server.gameName);
}

void updateServers(server *tmpserver, int port, char *host)
{

    char portstr[17];
    sprintf(portstr, "%d", port);
    strcpy(tmpserver->port, portstr);
    strcpy(tmpserver->host, host);

    printServer(*tmpserver);

    if( server_list.count == 0)
    {    
        server_list.servers[0] = *tmpserver;
        server_list.count++;
    }
    else
    {
        
        int i;
        int update = -1;
        for (i = 0; i < server_list.count; i++)
        {
            if (strcmp(server_list.servers[i].host, host) == 0 &&
                    strcmp(server_list.servers[i].port, portstr) == 0)
            {
                update = i;
                server_list.servers[i] = *tmpserver;
                //printf("Updated\n\n");
            }
        }
        if (update == -1)
        {
            server_list.servers[server_list.count] = *tmpserver;
            server_list.count++;
        }    
    }
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

	//Hard coded arguments at this point
	char port[17];
	
    server_list.count = 0;
    server temp_server;
    
	// Sets hints for binding socket
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

    getPort(port);
    printf("Master server is using port: %s\n\n", port);    

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
	
		printf("master server: waiting for packet...\n");

		// Save address sice and read with recvfrom
		sin_size = sizeof(their_addr);
		if ((numbytes = recvfrom(sockfd, inbuf, MAXDATASIZE-1, 0,
				(struct sockaddr *)&their_addr, &sin_size)) == -1) {
			perror("rcvfrom");
			exit(1);
		}

		int msgtype = getmessagetype(inbuf); // get messagetype

		// Address and port in readable form
		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),
				ipstr, sizeof(ipstr));
		p_port = ntohs(get_in_port((struct sockaddr *)&their_addr));
		printf("server: got packet from %s, %d\n",
				ipstr, p_port);
		printf("server: packet(%d) is %d bytes long\n\n", msgtype, numbytes);
		inbuf[numbytes] = '\0';

		// Join
		if (msgtype == UPDATESTATE)
		{
            // Server sent update state
            unpackUpdateState(inbuf, &temp_server);
            updateServers(&temp_server, p_port, ipstr);

		}
        else if (msgtype == GAMESQUERY)
        {
            // Client sent Games query
            lenout = packGameList(outbuf, &server_list);
        }
        else if (msgtype == SERVERQUERY)
        {
            // Client sent Server query
            lenout = packServerList(outbuf, &server_list);
        }

        if(lenout > 0){
            numbytes = sendto(sockfd, outbuf, lenout, 0, (struct sockaddr *) &their_addr, sin_size);
            memset(outbuf,'\0', MAXDATASIZE);
            printf("Sent %d bytes\n\n", lenout);
            lenout = 0;
		} 


	} // end while

	close(sockfd); // close socket

	return 0;
}

