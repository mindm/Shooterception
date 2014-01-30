/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include <stdlib.h>

// Enemy struct
typedef struct t_enemy {
    int xcoord; // X coordinate
    int ycoord; // Y coordinate
    int viewDirection; // Direction the enemy is facing and moving
    int health; // Enemy health - 0: Uninjured, 1: Injured, 2: Dead
    int following // ID of the player the enemy is following
} enemy;

// Player character stuct 
typedef struct t_player {
    int xcoord; // X coordinate
    int ycoord; // Y coordinate
    int viewDirection; // Direction the PC is facing, 0-359 degrees
    int health; // Three Health points, third hit kills
    int hasShot; // Has the player shot after sending the last clientState
    int isHost; // Is the player the host - 0: False, 1: True
} player;

// Game struct
typedef struct t_game {
    player playerList; // List of PCs in game
    int playerCount; // Number of players in game
    int enemyCount; // Number of enemies in game
    int levelNumber; // Number of the game level, defines level parameters
    int enemyLimit; // How many enemies current level will spawn
    int enemySpawnRate; // How quickly new enemies are spawned
    int enemyBaseSpeed; // Base speed for enemies
    int gameState; // 0: Waiting game, 1: inLobby, 2: inGame
} game;


// Function to update enemy location
void updateEnemyLocation (int, int);

// Function to update PC location
void updatePlayerLocation (int);

// Funtion to determine if enemy is shot - if PC has shot check if there are enemies on firing line
void checkHit (player*);

// Function to determine if player character and enemy collide
void checkCollision (player*, enemy*);

// Check end condition: all PCs are dead or enemyLimit and enemyCount are zero
int checkEnd (void);
