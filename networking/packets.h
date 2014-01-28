/* Joonas Blomberg 0342152 <joonas.blomberg@lut.fi>
 * packets.h
 *
 *
 *
 */

#ifndef PACKETS_H_
#define PACKETS_H_

#include "generic.h"

uint16_t getmessagetype(char *buf);
int packmessagetype(char *buf, uint16_t msgtype);

/* Client to server messages */
int packChatMessage(char *message);
int packCreateGame(char *gameName, int maxPlayers, char *playerName)
int packJoinGame(char *gameName, char *playerName)
int packStartGame();
int packClientState(int xcood, int ycood, int viewDirection,
        bool hasShot, int messageNumber, int timeReply);
int packClientExit();
int packAck();


/* Server to client messages */
int packLobbyState(); // Have to decide how to pack 1-4 namespace
int packGameStart(int gameLevel);
int packServerState(int playerCount, int enemyCount,
        int messageNumber, int timeSent); // Not ready
int packChatRelay(char * message);
int packError(int errorCode);
// int packAck();


/* Client to master messages */
int packGamesQuery();
int packServerQuery();


/* Master to client messages */
int packGameList(int gameCount, char *IP, int port, char *gameName, int maxPlayers);
int packServerName(int serverCount, char *IP, int port);


/* Server to master messages */
int packUpdateStats(int playerNumber);
int packServerState(int serverAction);



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

