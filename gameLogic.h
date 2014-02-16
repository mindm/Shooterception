/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define maxEnemies 20
#define COOLDOWN 0.5 // Cooldown for shooting

// View direction
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

// Enemy struct
typedef struct t_enemy {
    int xcoord; // X coordinate, upper left corner
    int ycoord; // Y coordinate, upper left corner
    int viewDirection; // Direction the enemy is facing and moving
    uint8_t health; // Enemy health - 0: Uninjured, 1: Injured, 2: Dead
    int following; // ID of the player the enemy is following
    int speed; // Enemy speed
} enemy;

// Player character stuct 
typedef struct t_player {
    int xcoord; // X coordinate, upper left corner
    int ycoord; // Y coordinate, upper left corner
    int viewDirection; // Direction the PC is facing, 0-359 degrees
    int health; // Three Health points, third hit kills
    uint8_t hasShot; // Has the player shot after sending the last clientState
    int isHost; // Is the player the host - 0: False, 1: True
    int playerNumber; // Player's number
    char* playerName; // Player's name
    //player_n *connectionInfo; // pointer to struct containing sockaddr_storage
} player;

// Game struct
typedef struct t_game {
    player playerList[3]; // List of PCs in game
    enemy enemyList[19]; // List of enemies in game
    int playerCount; // Number of players in game
    int enemyCount; // Number of enemies in game
    int levelNumber; // Number of the game level, defines level parameters
    int enemyLimit; // How many enemies current level will spawn
    int enemySpawnRate; // How quickly new enemies are spawned
    int enemyBaseSpeed; // Base speed for enemies
    int currentState; // 0: Waiting game, 1: inLobby, 2: inGame
} game;


// Function to update PC location
// Params: game state, xcoord, ycoord, viewDirection, hasShot
game* updatePlayerInfo (game*, int, int, int, int);

// Function to update enemy location
game* updateEnemyLocations (game*);

// Funtion to determine if enemy is shot - if PC has shot check if there are enemies on firing line
game* checkHit (game*);

// Function to determine if player character and enemy collide
game* checkCollision (game*);

// Check end condition: all PCs are dead or enemyLimit and enemyCount are zero
int checkEnd (game*);

// Relay chat messages to all clients
void relayChat (void); // Check if private message

// Send game state to all clients
void sendGameState(void);

// New game struct
// Params:
game* newGame(void);
void freeGame(game*);

// Start new game
int startGame(game*);

// Go to lobby state
void startLobby(game*);

// Add new player to existing game lobby
// Params: gameState struct, playerNumber, playerName
game* addPlayer(game*, int, char[16]);

// Set game state to "inGame" when host ready
game* hostReady(game*);

// Add enemies to gameState
//Params: game struct
game* addEnemy(game*);

//Function to set level parameters defined by level number
game* setLevelParameters(game*);

// Set players to initial positions
game* setPlayerLocations(game*);
