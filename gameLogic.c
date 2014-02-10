/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "gameLogic.h"

int startGame(game* gameState){

    while(1){
        // Main game logic loop

        // Update all player character locations
        //updatePlayerLocation();
        
        // Update all enemy locations
        //updateEnemyLocation();
        
        // Have player character's shot and do they hit enemies
        //checkHit();
        
        // Are player character and enemy colliding
        //checkCollision();
        
        // Check end condition
        //checkEnd();
        
        // Relay chat message to correct clients
        //relayChat();
        
        // Send game state to all clients
        //sendGameState();
        
        // Framecap - implement without SDL_Delay
        /*if(delta_time() < 1000/FPS) {
            SDL_Delay((1000/FPS) - delta_time());
        } */
    }

    return 0;
}

// Update PC location
void updatePlayerLocation (void){


}

// Update enemy location
void updateEnemyLocation (void){


}

// Determine if enemy is shot - if PC has shot check if there are enemies on firing line
void checkHit (void){


}

// Function to determine if player character and enemy collide
void checkCollision (void){


}


// Check end condition: all PCs are dead or enemyLimit and enemyCount are zero
void checkEnd (void){


}

// Relay chat messages to all clients
void relayChat (void){ // Check if private message


}

// New game state
game* newGame(void) {

    // Allocate game struct
    game *gameState = (game*)malloc(sizeof(game));

    // Set the game state
    //gameState->playerList = NULL; // Clear player list
    //gameState->enemyList = NULL; // Clear enemy list
    gameState->playerCount = 1; // Number of players after the host joins
    gameState->enemyCount = 0; // Number of enemies when game is created
    gameState->levelNumber = 1; // Number of the game level (1-3), new game starts from 1
    gameState->enemyLimit = 10; // Enemy spawn limit for level 0
    gameState->enemySpawnRate = 1; // Enemy spawn limit for level 1
    gameState->enemyBaseSpeed = 1; // Enemy base speed for level 1
    gameState->currentState = 0; // 0: Waiting game, 1: inLobby, 2: inGame

    return gameState;
}

// Free game state
void freeGame(game* gameState) {
    if(gameState) {
        free(gameState);
    }
}

game* addPlayer(game* gameState, int isHost, int playerNumber, char* playerName){

    // Allocate new player struct
    player *newPlayer = (player*)malloc(sizeof(player));
    
    // Set new player parameters
    newPlayer->xcoord = 0 ; // Initial X coordinate
    newPlayer->ycoord = 0; // Initial Y coordinate
    newPlayer->viewDirection = 0; // Initial direction the PC is facing
    newPlayer->health = 3; // Initial Health points
    newPlayer->hasShot = 0; // Player has not shot
    newPlayer->isHost = isHost; // Is the player the host - 0: False, 1: True
    newPlayer->playerNumber = playerNumber; // Player's number
    newPlayer->playerName = playerName; // Player's name
    
    //memset(gameState->playerList,newPlayer,sizeof(newPlayer));
    //gameState->playerList[newPlayer->playerNumber] = newPlayer; // Allocate new player
    
    return gameState;
}

int main (void){

    game* gameState = NULL;

    // Game state for new game
    gameState = newGame();  

    // Change to inLobby state
    gameState->currentState = 1;

    
    // Check if in lobby state
    if(gameState->currentState == 1){
    
        // Waiting in lobby for players and game start
        while(1){
        
            // Clean temporary variable
            player* newPlayer = NULL;
            
            // Add player to game
            //addPlayer(game* gameState, int isHost, int playerNumber, char playerName[16]);
            gameState = addPlayer(gameState, 1, 0, "asd");
            
            // Start the game when host is ready
            //if(isHostReady() == 1){
            //    break;
            //}
            
            // Wait 2 seconds before retry
            sleep(2);
        }
    }

     
     
     
    // Start the game
    startGame(gameState);
    
    // Cleanup after the game ends
    freeGame(gameState);
    
    return 0;
}


// Go to lobby state
void startLobby(game* gameState){


}

/*


void move_enemies() {

if(enemy->health <= 0)
return;

int x1, y1, x2, y2;

x1 = player.b.x;
y1 = player.b.y;

x2 = enemy->b.x;
y2 = enemy->b.y;

//calculate x distance to player, move towards
if((x1 - x2) < 0) {
enemy->dir = LEFT;
enemy->b.x -= EN_VEL;
}
else {
enemy->dir = RIGHT;
enemy->b.x += EN_VEL;
}
//calculate y distance, move towards
if((y1 - y2) < 0){
enemy->dir = UP;
enemy->b.y -= EN_VEL;
}
else {
enemy->dir = DOWN;
enemy->b.y += EN_VEL;
}
}

void move_player() {

//left/right
player.b.x += player.xVel;

//screen left/right border
if((player.b.x < 0) || (player.b.x + PC_DIMS > SCREEN_WIDTH)) {
//move back
player.b.x -= player.xVel;
}

// up/down
player.b.y += player.yVel;

//screen top/bottom border
if((player.b.y < 0) || ( player.b.y + PC_DIMS > SCREEN_HEIGHT)) {
//move back
player.b.y -= player.yVel;
}
}

int check_collision(SDL_Rect A, SDL_Rect B) {

    int leftA, rightA, topA, bottomA;
    int leftB, rightB, topB, bottomB;

    //sides of rect A
    leftA = A.x;
    rightA = A.x + A.w;
    topA = A.y;
    bottomA = A.y + A.h;

    //sides of rect B
    leftB = B.x;
    rightB = B.x + B.w;
    topB = B.y;
    bottomB = B.y + B.h;

    //if any of the sides from A are outside of B
    if(bottomA <= topB)
        return 0;

    if(topA >= bottomB)
        return 0;

    if(rightA <= leftB)
        return 0;

    if(leftA >= rightB)
        return 0;

    //if none of the sides from A are outside B
    return 1;
} */
