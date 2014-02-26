/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#ifndef __TCPEX_GEN_H
#define __TCPEX_GEN_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define MAXENEMIES 20
#define COOLDOWN 0.5 // Cooldown for shooting

// View directions
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define GAMENAME_LENGTH 16
#define PLAYERNAME_LENGTH 16
#define CHATMESSAGE_LENGTH 100

// Packet types
// Client
#define CHATMESSAGE 6
#define CREATEGAME 1
#define JOINGAME 2
#define STARTGAME 3
#define CLIENTSTATE 4
#define CLIENTEXIT 5
// Server
#define LOBBYSTATE 21
#define GAMESTART 22
#define SERVERSTATE 23
#define CHATRELAY 24
#define ERROR 100
//Both
#define ACK 0
//Master-client
#define GAMESQUERY 31
#define SERVERQUERY 32
#define GAMELIST 33
#define SERVERLIST 34
//master-server
#define UPDATESTATE 41

// connectionInfo struct
typedef struct player_n
{
	char address[INET6_ADDRSTRLEN];
	int port;
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
} player_n;

// Enemy struct
typedef struct t_enemy {
    int xcoord; // X coordinate, upper left corner
    int ycoord; // Y coordinate, upper left corner
    int viewDirection; // Direction the enemy is facing and moving
    uint8_t health; // Enemy health - 0: Uninjured, 1: Injured, 2: Dead
    int following; // ID of the player the enemy is following
    int speed; // Enemy speed
    int isShot; // Has the enemy been shot?
} enemy;

// Player character stuct 
typedef struct t_player {
    int xcoord; // X coordinate, upper left corner
    int ycoord; // Y coordinate, upper left corner
    int viewDirection; // Direction the PC is facing, 0-359 degrees
    int health; // Three Health points, third hit kills
    uint8_t hasShot; // Has the player shot after sending the last clientState
    unsigned long shotCooldown; // PCs can shoot 2/s
    int canShoot; // Cooldown not running, 0: False, 1: True
    int isHost; // Is the player the host - 0: False, 1: True
    int playerNumber; // Player's number
    char playerName[17]; // Player's name
    player_n connectionInfo; // pointer to struct containing sockaddr_storage
    int isColliding; // Is the PC colliding with enemy?
} player;

// Game struct
typedef struct t_game {
    player playerList[4]; // List of PCs in game
    enemy enemyList[20]; // List of enemies in game
    int playerCount; // Number of players in game
    int enemyCount; // Number of enemies in game
    int levelNumber; // Number of the game level, defines level parameters
    int enemyLimit; // How many enemies current level will spawn
    unsigned long enemySpawnTimer; // When enemy last spawned
    long enemySpawnRate; // How quickly new enemies are spawned
    int enemyBaseSpeed; // Base speed for enemies
    int currentState; // 0: Waiting game, 1: inLobby, 2: inGame
    int maxPlayers; // Maximum amount of players, 1-4
    char gameName[17]; // Game name
    unsigned long updateTimer; // gameState update timer
    int msgNumber; // State update number
} game;

//Struct for server info
typedef struct server {
    char host[46];
    char port[17];
    int serverState;
    int playerNumber;
    int maxPlayers;
    char gameName[17];
} server;

// Holds different servers
typedef struct serverList
{   
    int count;
    server servers[20];
} serverList;

#endif
