/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "gameLogic.h"

int startGame(game* gameState){

    int spawnTimer = 0;
    
    srand(time(NULL)); // Generate random seed for rand()
    
    // Set level parameters defined by LevelNumber
    gameState = setLevelParameters(gameState);

    // Set initial players locations
    gameState = setPlayerLocations(gameState);

    while(1){
        // Main game logic loop

        // Spawn enemy if spawn rate allows and maxEnemies is not full
        if(spawnTimer >= gameState->enemySpawnRate && gameState->enemyCount < maxEnemies){
            // Spawn enemy to random border coordinate with level base speed and PC to follow
            gameState = addEnemy(gameState);
            gameState->enemyCount += 1;
        }

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
        
        sleep(1);
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
    gameState->playerCount = 1; // Number of players after the host joins
    gameState->enemyCount = 0; // Number of enemies on the game board
    gameState->levelNumber = 1; // Number of the game level (1-3), new game starts from 1

    return gameState;
}

// Free game state
void freeGame(game* gameState) {
    if(gameState) {
        free(gameState);
    }
}

game* addPlayer(game* gameState, int playerNumber, char* playerName){

    // Allocate new player struct
    //player *newPlayer = (player*)malloc(sizeof(player));
    player newPlayer[0];
    
    // Set new player parameters
    newPlayer[0].xcoord = 0 ; // Initial X coordinate
    newPlayer[0].ycoord = 0; // Initial Y coordinate
    newPlayer[0].viewDirection = 0; // Initial direction the PC is facing
    newPlayer[0].health = 3; // Initial Health points
    newPlayer[0].hasShot = 0; // Player has not shot
    
    if(playerNumber==0){
        newPlayer[0].isHost = 1; // First player, mark as host
    }
    else{
        newPlayer[0].isHost = 0;
    }
    
    newPlayer[0].playerNumber = playerNumber; // Player's number
    newPlayer[0].playerName = playerName; // Player's name
    
    gameState->playerList[playerNumber] = newPlayer[0]; // Allocate new player
    
    return gameState;
}

game* hostReady(game* gameState){
    gameState->currentState = 2;
    return gameState;
}

game* addEnemy(game* gameState){

    int randomPlayer = rand() % gameState->playerCount+1; // Choose random player to follow
    int randomXborder = rand() % 2; // Choose random border from x axis
    int randomYborder = rand() % 2; // Choose random border from y axis
    int randomXcoord = rand() % 800; // Choose random border from x axis
    int randomYcoord = rand() % 800; // Choose random border from y axis
    
    enemy newEnemy[0];
    
    // Set new enemy parameters
    
    // Choose spawn border
    if(randomXcoord == 0 && randomYcoord == 0){ // Top border
        newEnemy[0].xcoord = randomXcoord;
        newEnemy[0].ycoord = 0;
    }
    if(randomXcoord == 0 && randomYcoord == 1){ // Right border
        newEnemy[0].xcoord = 800;
        newEnemy[0].ycoord = randomYcoord;
    }
    if(randomXcoord == 1 && randomYcoord == 1){ // Bottom border
        newEnemy[0].xcoord = randomXcoord;
        newEnemy[0].ycoord = 800;
    }
    if(randomXcoord == 1 && randomYcoord == 0){ // Left border
        newEnemy[0].xcoord = 0;
        newEnemy[0].ycoord = randomYcoord;
    }        
    newEnemy[0].health = 3; // Initial Health points
   
    newEnemy[0].following = randomPlayer; // Player who enemy is following
    
    // TODO Changing base speed
    newEnemy[0].speed = gameState->enemyBaseSpeed; // Player who enemy is following
    
    gameState->enemyList[gameState->enemyCount] = newEnemy[0]; // Allocate new enemy
        
    return gameState;
}

game* setLevelParameters(game* gameState){

    // Set game parameters for game level
    if(gameState->levelNumber == 1){
        gameState->enemyLimit = 10; // How many enemies are spawned
        gameState->enemySpawnRate = 1; // How fast the enemis are spawned
        gameState->enemyBaseSpeed = 1; // How fast the enemies _atleast_ move
    } 
    else if(gameState->levelNumber == 2){
        gameState->enemyLimit = 15; // How many enemies are spawned
        gameState->enemySpawnRate = 1; // How fast the enemies are spawned
        gameState->enemyBaseSpeed = 2; // How fast the enemies _atleast_ move
    
    } 
    else if(gameState->levelNumber ==3){
        gameState->enemyLimit = 20; // How many enemies are spawned
        gameState->enemySpawnRate = 1; // How fast the enemies are spawned
        gameState->enemyBaseSpeed = 3; // How fast the enemies _atleast_ move
    }
    
    return gameState;
}

game* setPlayerLocations(game* gameState){

    for(int i = 0; i <= gameState->playerCount; i++){
        if(gameState->playerList[i].playerNumber == 1){
            gameState->playerList[i].xcoord = 400;
            gameState->playerList[i].ycoord = 360;            
        }
        else if(gameState->playerList[i].playerNumber == 2){
            gameState->playerList[i].xcoord = 440;
            gameState->playerList[i].ycoord = 400;            
        }
        else if(gameState->playerList[i].playerNumber == 3){
            gameState->playerList[i].xcoord = 400;
            gameState->playerList[i].ycoord = 440;           
        }
        else if(gameState->playerList[i].playerNumber == 4){
            gameState->playerList[i].xcoord = 360;
            gameState->playerList[i].ycoord = 400;           
        }
    
    }
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
            //addPlayer(game* gameState, int playerNumber, char playerName[16]);
            gameState = addPlayer(gameState, 0, "asd");
            
            // First player sends startGame and server calls hostReady function
            // Function sets currentState to 2, "inGame"
            if(gameState->currentState == 2){
                break;
            }
            
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
