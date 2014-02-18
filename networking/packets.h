/*
* CT30A5002 Games and Networking 2013-14
* Authors:
* Arttu Hanska (0342181) - arttu.hanska@lut.fi
* Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
* Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#ifndef PACKETS_H_
#define PACKETS_H_

//#include "generic.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <netinet/in.h>
#include "../generic.h"

/* ##### Packing functions #####*/

/*typedef struct t_player {
    int xcoord; // X coordinate, upper left corner
    int ycoord; // Y coordinate, upper left corner
    int viewDirection; // Direction the PC is facing, 0-359 degrees
    int health; // Three Health points, third hit kills
    uint8_t hasShot; // Has the player shot after sending the last clientState
    int isHost; // Is the player the host - 0: False, 1: True
    int playerNumber; // Player's number
    char* playerName; // Player's name
} player; */

// Player character stuct 
typedef struct t_player {
    int xcoord; // X coordinate, upper left corner
    int ycoord; // Y coordinate, upper left corner
    int viewDirection; // Direction the PC is facing, 0-359 degrees
    int health; // Three Health points, third hit kills
    uint8_t hasShot; // Has the player shot after sending the last clientState
    int isHost; // Is the player the host - 0: False, 1: True
    int playerNumber; // Player's number
    char playerName[16]; // Player's name
    player_n* connectionInfo; // pointer to struct containing sockaddr_storage
} player;

/* Generic */
//uint16_t getmessagetype(char *buf);
int packMessageType(char *buf, uint8_t msgtype);
int packAck(char *buf);
uint8_t getmessagetype(char *buf);
void cutSpace(char *ptr);


/* Client to server messages */
int packChatMessage(char *buf, char *message);
int packCreateGame(char *buf, char *gameName, int maxPlayers, char *playerName);
int packJoinGame(char *buf, char *gameName, char *playerName);
int packStartGame(char *buf);
int packClientState(char *buf, player playerInfo, int messageNumber/*, int timeReply*/);
int packClientExit(char *buf);


/* Server to client messages */
int packLobbyState(char *buf, char *player1, char *player2, char *player3, char *player4); // Have to decide how to pack 1-4 namespace
int packGameStart(char *buf, int gameLevel);
int packServerState(char *buf, int playerCount, int enemyCount,
        int messageNumber, int timeSent); // Not ready
int packChatRelay(char *buf, char *message);
int packError(char *buf, int errorCode);


/* Client to master messages */
int packGamesQuery();
int packServerQuery();


/* Master to client messages */
int packGameList(int gameCount, char *IP, int port, char *gameName, int maxPlayers);
int packServerName(int serverCount, char *IP, int port);


/* Server to master messages */
int packUpdateStats(int playerNumber);
//int packServerState(int serverAction);


/* ##### UnPacking functions #####*/

/*Messages containing only messageType need not be unpacked*/

/* Generic */
uint8_t getMessageType(char *buf);

/* Client to server messages */
void unpackChatMessage(char *buf, char *message);
void unpackCreateGame(char *buf, char *gameName, int *maxPlayers, char *playerName);
void unpackJoinGame(char *buf, char *gameName, char *playerName);
void unpackClientState(char *buf, player *playerInfo, int *messageNumber); // Maybe a struct here??



/* Server to client messages */
void unpackLobbyState(char *buf, char *name1, char *name2, char *name3, char *name4); // Not ready
void unpackGameStart(char *buf, int *gameLevel);
int unpackServerState(char *buf, int playerCount, int enemyCount,
        int messageNumber, int timeSent); // Struct here too
void unpackChatRelay(char *buf, char *message);
int unpackError(char *buf, int *errorCode);


/* Client to master messages */
//None to unpack


/* Master to client messages */
int unpackGameList(int *gameCount, char *IP, int *port, char *gameName, int *maxPlayers); // Struct would be nice
int unpackServerName(int *serverCount, char *IP, int *port);


/* Server to master messages */
int unpackUpdateStats(int *playerNumber);
//int unpackServerState(int *serverAction);





/*
int packcolumn(char *buf, int column); //message 5
int packjoin(char *buf); // message 0
int packready(char *buf); // message 2
int packquit(char *buf); // message 8
void unpackok(char *buf, int *count, int *id);
void unpackstart(char *buf, int *cols, int *rows, int *players);
void unpackarea(char *buf, char *area, int cols, int rows);
void unpackwinner(char *buf, int *winner);
void unpackerror(char *buf, int *errtype, int numbytes, char *msg);


int unpackcolumn(char *buf);
int packok(char *buf, int count, int id); // message 1
int packstart(char *buf, int cols, int rows, int players); //message 3
int packturn(char *buf); //message 4
int packarea(char *buf, char *area, int rows, int cols); //message 6
int packwinner(char *buf, int winner); //message 7
int packerror(char *buf, int code, char *message); // message 1000
*/
#endif /* PACKETS_H_ */

