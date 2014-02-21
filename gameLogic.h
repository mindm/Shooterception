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

// Check end condition: all PCs are dead or enemyLimit and enemyCount are zero
int checkEnd (game*);

// Relay chat messages to all clients
void relayChat (void); // Check if private message

// Send game state to all clients
void sendGameState(void);

// New game struct
// Params: gameName, maxPlayers
game* newGame(char[16], int);
void freeGame(game*);

// Start new game
int startGame(game*, char[MAXDATASIZE]);

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
struct timeval getCurrentTime(void);

// Check level spawnTimer
game* checkSpawnTimer(game*);

#endif /* GAMELOGIC_H_ */
