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
        
        // Update all player characters location, view direction and if the PC has shot
        //gameState = updatePlayerInformation(gameState, xcoord, ycoord, viewDirection, hasShot);
        gameState = updatePlayerInfo(gameState, 200, 200, UP, 0);
        
        // Update all enemy locations
        gameState = updateEnemyLocations(gameState);
        
        // Have player character's shot and do they hit enemies
        gameState = checkHit(gameState);
        
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
game* updatePlayerInfo (game* gameState, int xcoord, int ycoord, int viewDirection, int hasShot){

    int i = 0;
/*  TODO: Implement with network integration

    // Determine player number by comparing passed connectionInfo to variable in struct
    for(i=0; i<gameState.playerCount;i++){
        if(gameState->playerList[i].connectionInfo == passed connectionInfo{
            break;
    } 
*/

    // Check that PC is within game boundaries and update coordinates
    if(xcoord==0){
        gameState->playerList[i].xcoord = 0;
    }
    else if(xcoord==800){     
        gameState->playerList[i].xcoord = 800;
    }
    else{
        gameState->playerList[i].xcoord = xcoord;
    }
    if(ycoord==0){
        gameState->playerList[i].ycoord = 0;
    }
    else if(ycoord==800){     
        gameState->playerList[i].ycoord = 800;
    }
    else{
        gameState->playerList[i].ycoord = ycoord;
    }    

    gameState->playerList[i].viewDirection = viewDirection;
    gameState->playerList[i].hasShot = hasShot;

    return gameState;
}

// Update enemy location
game* updateEnemyLocations (game* gameState){

    for(int i = 0; i<gameState->enemyCount;i++){
        // Calculate enemy distance to followed PC and move towards
        if((gameState->playerList[gameState->enemyList[i].following].xcoord - gameState->enemyList[i].xcoord) < 0) {
            gameState->enemyList[i].viewDirection = LEFT;            
            gameState->enemyList[i].xcoord -= gameState->enemyList[i].speed;
        } else{
            gameState->enemyList[i].viewDirection = RIGHT;             
            gameState->enemyList[i].xcoord += gameState->enemyList[i].speed;        
        }
        if((gameState->playerList[gameState->enemyList[i].following].ycoord - gameState->enemyList[i].ycoord) < 0) {
            gameState->enemyList[i].viewDirection = UP;                        
            gameState->enemyList[i].ycoord -= gameState->enemyList[i].speed;      
        } else{
            gameState->enemyList[i].viewDirection = DOWN;      
            gameState->enemyList[i].ycoord += gameState->enemyList[i].speed;       
        }
    }

    return gameState;
}

// Determine if enemy is shot - if PC has shot check if there are enemies on firing line
game* checkHit (game* gameState){

    int i = 0; // TODO Player number - this information is passed from networking
    int k = 0;
    int candidateNumber = 0;
    int candidateDistance = 0;
    enemy hitCandidates[19]; // Used to determine closest enemy
        
    // Check if PC hit enemy
    for(int j=0;j<gameState->enemyCount;j++){
   
        // PC facing UP - same xcoord, PC has higher ycoord
        if(gameState->playerList[i].viewDirection == UP){
            if(gameState->playerList[i].xcoord == gameState->enemyList[j].xcoord && gameState->playerList[i].ycoord >= gameState->enemyList[j].ycoord){
                // Enemy in hit line, add to hit candidates
                hitCandidates[k] = gameState->enemyList[j];
                k++;
            }
        }
        // PC facing DOWN - same xcoord, PC has lower ycoord           
        else if(gameState->playerList[i].viewDirection == DOWN){
            if(gameState->playerList[i].xcoord == gameState->enemyList[j].xcoord && gameState->playerList[i].ycoord <= gameState->enemyList[j].ycoord){
                // Enemy in hit line, add to hit candidates
                hitCandidates[k] = gameState->enemyList[j];
                k++;
            }                
        }
        // PC facing RIGHT - same ycoord, PC has lower xcoord                
        else if(gameState->playerList[i].viewDirection == RIGHT){
            if(gameState->playerList[i].ycoord == gameState->enemyList[j].ycoord && gameState->playerList[i].xcoord <= gameState->enemyList[j].xcoord){
                // Enemy in hit line, add to hit candidates
                hitCandidates[k] = gameState->enemyList[j];
                k++;
            }                 
        }
        // PC facing LEFT - same ycoord, PC has higher xcoord              
        else if(gameState->playerList[i].viewDirection == LEFT){
            if(gameState->playerList[i].ycoord == gameState->enemyList[j].ycoord && gameState->playerList[i].xcoord >= gameState->enemyList[j].xcoord){
                // Enemy in hit line, add to hit candidates
                hitCandidates[k] = gameState->enemyList[j];
                k++;
            }             
        }           
    }
    
    // Determine closest enemy in range


    // PC facing UP - closest enemy has highest ycoord 
    if(gameState->playerList[i].viewDirection == UP){  
        candidateDistance = 0;
        for(int z=0;z<k;z++){     
            if(hitCandidates[z].ycoord >= candidateDistance){
                candidateDistance = hitCandidates[z].ycoord;
                candidateNumber = z;
            }
        }
    }
    // PC facing DOWN - closest enemy has lowest ycoord
    else if(gameState->playerList[i].viewDirection == DOWN){   
        candidateDistance = 800;
        for(int z=0;z<k;z++){       
            if(hitCandidates[z].ycoord <= candidateDistance){
                candidateDistance = hitCandidates[z].ycoord;
                candidateNumber = z;                
            }
        }        
    }    
    // PC facing RIGHT - closest enemy has lowest xcoord
    else if(gameState->playerList[i].viewDirection == RIGHT){    
        candidateDistance = 800;
        for(int z=0;z<k;z++){      
            if(hitCandidates[z].xcoord <= candidateDistance){
                candidateDistance = hitCandidates[z].xcoord;
                candidateNumber = z;                
            }
        }    
    }
    // PC facing LEFT - closest enemy has highest xcoord
    else if(gameState->playerList[i].viewDirection == LEFT){  
        candidateDistance = 0;
        for(int z=0;z<k;z++){        
            if(hitCandidates[z].xcoord >= candidateDistance){
                candidateDistance = hitCandidates[z].xcoord;
                candidateNumber = z;                
            }
       }     
    }     
    // Enemy hit, reduce Health points by 1
    gameState->enemyList[candidateNumber].health -= 1;                                                 

    // TODO: Add enemy death

    return gameState;
}

// Function to determine if player character and enemy collide
game* checkCollision (game* gameState){
    
    // Loop through enemies and PCs and check if they're in same coordinate
    for(int i=0;i<gameState->enemyCount;i++){
        for(int j=0;j<gameState->playerCount;j++){
            if(gameState->enemyList[i].xcoord == gameState->playerList[j].xcoord && gameState->enemyList[i].ycoord == gameState->playerList[j].ycoord){
                // Enemy and PC collide, reduce PC's Health points by 1
                gameState->playerList[j].health -= 1;
            }
        }
    }

    return gameState;
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
    if(randomXborder == 0 && randomYborder == 0){ // Top border
        newEnemy[0].xcoord = randomXcoord;
        newEnemy[0].ycoord = 0;
        newEnemy[0].viewDirection = DOWN;
    }
    if(randomXborder == 0 && randomYborder == 1){ // Right border
        newEnemy[0].xcoord = 800;
        newEnemy[0].ycoord = randomYcoord;
        newEnemy[0].viewDirection = LEFT;
    }
    if(randomXborder == 1 && randomYborder == 1){ // Bottom border
        newEnemy[0].xcoord = randomXcoord;
        newEnemy[0].ycoord = 800;
        newEnemy[0].viewDirection = UP;
    }
    if(randomXborder == 1 && randomYborder == 0){ // Left border
        newEnemy[0].xcoord = 0;
        newEnemy[0].ycoord = randomYborder;
        newEnemy[0].viewDirection = RIGHT;
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

    // TODO Move these function calls to networking!

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
            //player* newPlayer = NULL;
            
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
