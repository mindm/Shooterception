/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#ifndef GAMELOGIC_H_
#define GAMELOGIC_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h> 
#include <unistd.h>
#include <ctype.h>
#include <math.h>

#include "generic.h"
#include "networking/packets.h"
#include "networking/networking_server.h"



// Function to update PC location
// Params: game state, connection info, xcoord, ycoord, viewDirection, hasShot
game* updatePlayerInfo (game*, player_n*, int, int, int, int);

// Function to update enemy location
game* updateEnemyLocations (game*);

void updateLobby(game*, char*);

// Funtion to determine if enemy is shot - if PC has shot check if there are enemies on firing line
// Params: game state, playerNumber
game* checkHit (game*, int);

// Function to determine if player character and enemy collide
game* checkCollision (game*);
game* checkCollisionDistance(game *);

// Check end condition: all PCs are dead or enemyLimit and enemyCount are zero
int checkEnd (game*);

// New game struct
// Params: game state, gameName, maxPlayers
game* newGame(game*, char[16], int);
void freeGame(game*);

// Start new game
// Params: game state, outbuffer, levelNumber
int startGame(game*, char[MAXDATASIZE], int);

// Go to lobby state
void startLobby(game*);

// Add new player to existing game lobby
// Params: gameState struct, connectionInfo, playerName
game* addPlayer(game*, player_n*, char[16]);

// Add enemies to gameState
//Params: game struct
game* addEnemy(game*);

//Function to set level parameters defined by level number
game* setLevelParameters(game*);

// Set players to initial positions
game* setPlayerLocations(game*);

// Determine player number based on connection info
int getPlayerNumber(game*, player_n*);

// Check shotCooldown for player
// Params: game state, playerNumber
game* checkShotCooldown(game*, int);

// Get current time for timers
//struct timeval getCurrentTime(void);
struct timespec getCurrentTime(void);

// Check level spawnTimer
game* checkSpawnTimer(game*);

// Reset enemies isShot back to 0
game* resetEnemyHits(game*);

// Reset players isColliding back to 0
//game* resetPlayerCollisions(game*);

// Create initial game state when server starts
game* initGame(void);

// Remove player from game
game* removePlayer(game*, player_n*);
void relayChatMessage(game*,  char[MAXDATASIZE], char[CHATMESSAGE_LENGTH]);

// Send game state to all clients
// Params: game state, outbuffer
game* sendGameState(game*, char[MAXDATASIZE]);

// Players won, next level
game* nextLevel(game*, char[MAXDATASIZE]);

// Players lost, reset to first level
game* resetGame(game*, char[MAXDATASIZE]);

// Check if PC is invulnerable after last collision
// Params: game state, playerNumber
game* checkCollissionCooldown(game*, int);

#endif /* GAMELOGIC_H_ */
