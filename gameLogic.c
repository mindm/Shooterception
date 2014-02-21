/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "gameLogic.h"

int startGame(game* gameState, char outbuf[MAXDATASIZE]){

    // Change to state inGame
    gameState->currentState = 2;
    
    srand(time(NULL)); // Generate random seed for rand()
    
    // Set level parameters defined by LevelNumber
    gameState = setLevelParameters(gameState);

    // Set initial players locations
    gameState = setPlayerLocations(gameState);
    
    int size = packStartGame(outbuf);
    setLenout(size);

    return 0;
}

// Update PC location
game* updatePlayerInfo (game* gameState, player_n* connectionInfo, int xcoord, int ycoord, int viewDirection, int hasShot){

     int playerNumber = getPlayerNumber(gameState, connectionInfo);

    // Check that PC is within game boundaries and update coordinates
    if(xcoord==0){
        gameState->playerList[playerNumber].xcoord = 0;
    }
    else if(xcoord==800){     
        gameState->playerList[playerNumber].xcoord = 800;
    }
    else{
        gameState->playerList[playerNumber].xcoord = xcoord;
    }
    if(ycoord==0){
        gameState->playerList[playerNumber].ycoord = 0;
    }
    else if(ycoord==800){     
        gameState->playerList[playerNumber].ycoord = 800;
    }
    else{
        gameState->playerList[playerNumber].ycoord = ycoord;
    }    

    gameState->playerList[playerNumber].viewDirection = viewDirection;
    gameState->playerList[playerNumber].hasShot = hasShot;

    gameState = checkShotCooldown(gameState, playerNumber);

    // If PC has shot and cooldown timer isn't running, check for hit
    if(gameState->playerList[playerNumber].hasShot == 1 && gameState->playerList[playerNumber].shotCooldown == 0){
        gameState = checkHit(gameState, playerNumber);   
        gameState->playerList[playerNumber].shotCooldown = COOLDOWN;    
    }  

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
game* checkHit (game* gameState, int playerNumber){

    int k = 0;
    int candidateNumber = 0;
    int candidateDistance = 0;
    enemy hitCandidates[19]; // Used to determine closest enemy
        
    // Check if PC hit enemy
    for(int j=0;j<gameState->enemyCount;j++){
   
        // PC facing UP - same xcoord, PC has higher ycoord
        if(gameState->playerList[playerNumber].viewDirection == UP){
            if(gameState->playerList[playerNumber].xcoord == gameState->enemyList[j].xcoord && gameState->playerList[playerNumber].ycoord >= gameState->enemyList[j].ycoord){
                // Enemy in hit line, add to hit candidates
                hitCandidates[k] = gameState->enemyList[j];
                k++;
            }
        }
        // PC facing DOWN - same xcoord, PC has lower ycoord           
        else if(gameState->playerList[playerNumber].viewDirection == DOWN){
            if(gameState->playerList[playerNumber].xcoord == gameState->enemyList[j].xcoord && gameState->playerList[playerNumber].ycoord <= gameState->enemyList[j].ycoord){
                // Enemy in hit line, add to hit candidates
                hitCandidates[k] = gameState->enemyList[j];
                k++;
            }                
        }
        // PC facing RIGHT - same ycoord, PC has lower xcoord                
        else if(gameState->playerList[playerNumber].viewDirection == RIGHT){
            if(gameState->playerList[playerNumber].ycoord == gameState->enemyList[j].ycoord && gameState->playerList[playerNumber].xcoord <= gameState->enemyList[j].xcoord){
                // Enemy in hit line, add to hit candidates
                hitCandidates[k] = gameState->enemyList[j];
                k++;
            }                 
        }
        // PC facing LEFT - same ycoord, PC has higher xcoord              
        else if(gameState->playerList[playerNumber].viewDirection == LEFT){
            if(gameState->playerList[playerNumber].ycoord == gameState->enemyList[j].ycoord && gameState->playerList[playerNumber].xcoord >= gameState->enemyList[j].xcoord){
                // Enemy in hit line, add to hit candidates
                hitCandidates[k] = gameState->enemyList[j];
                k++;
            }             
        }           
    }
    
    // Determine closest enemy in range


    // PC facing UP - closest enemy has highest ycoord 
    if(gameState->playerList[playerNumber].viewDirection == UP){  
        candidateDistance = 0;
        for(int z=0;z<k;z++){     
            if(hitCandidates[z].ycoord >= candidateDistance){
                candidateDistance = hitCandidates[z].ycoord;
                candidateNumber = z;
            }
        }
    }
    // PC facing DOWN - closest enemy has lowest ycoord
    else if(gameState->playerList[playerNumber].viewDirection == DOWN){   
        candidateDistance = 800;
        for(int z=0;z<k;z++){       
            if(hitCandidates[z].ycoord <= candidateDistance){
                candidateDistance = hitCandidates[z].ycoord;
                candidateNumber = z;                
            }
        }        
    }    
    // PC facing RIGHT - closest enemy has lowest xcoord
    else if(gameState->playerList[playerNumber].viewDirection == RIGHT){    
        candidateDistance = 800;
        for(int z=0;z<k;z++){      
            if(hitCandidates[z].xcoord <= candidateDistance){
                candidateDistance = hitCandidates[z].xcoord;
                candidateNumber = z;                
            }
        }    
    }
    // PC facing LEFT - closest enemy has highest xcoord
    else if(gameState->playerList[playerNumber].viewDirection == LEFT){  
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
    
    // Check if enemy dies
    if(gameState->enemyList[candidateNumber].health == 0){
        // Overwrite dead enemy and compress the array
        for(int candidateNumber; candidateNumber < gameState->enemyCount; candidateNumber++){
            gameState->enemyList[candidateNumber] = gameState->enemyList[candidateNumber+1];
        }
        gameState->enemyCount -= 1;
        gameState->enemyLimit -= 1;
    }                             

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
                // Check if PC dies
                if(gameState->playerList[j].health == 0){
                    // TODO: What happens when PC's health goes to 0?
                }
            }
        }
    }


    return gameState;
}

// Check end condition: all PCs are dead or enemyLimit and enemyCount are zero
int checkEnd (game* gameState){

    int deadPlayers = 0;
    
    // All enemies defeated, level completed
    if(gameState->enemyLimit == 0){
        return 1;
    }
    
    for(int i=0;i<=gameState->playerCount;i++){
        if(gameState->playerList[i].health == 0){
            deadPlayers += 1;
            // All PCs defeated, game lost
            if (deadPlayers == gameState->playerCount){
                return 2;
            }
        }
    }
    
    // Game continues
    return 0;

}

// Relay chat messages to all clients
void relayChat (void){ // Check if private message


}

// New game state
game* newGame(char* gameName, int maxPlayers) {

    // Allocate game struct
    game *gameState = (game*)malloc(sizeof(game));

    // Set the game state
    // TODO: Format playerList and enemyList
    gameState->playerCount = 1; // Number of players after the host joins
    gameState->enemyCount = 0; // Number of enemies on the game board
    gameState->levelNumber = 1; // Number of the game level (1-3), new game starts from 1
    memcpy(gameState->gameName, gameName, GAMENAME_LENGTH);
    gameState->maxPlayers = maxPlayers;

    return gameState;
}

// Free game state
void freeGame(game* gameState) {
    if(gameState) {
        free(gameState);
    }
}

game* addPlayer(game* gameState, player_n* connectionInfo, char* playerName){

    // Check if player limit is full
    if(gameState->playerCount < gameState->maxPlayers){
        player newPlayer[1];
        
        // Get player number from connectionInfo
        int playerNumber = getPlayerNumber(gameState, connectionInfo);

        // Set new player parameters
        newPlayer[0].xcoord = 0 ; // Initial X coordinate
        newPlayer[0].ycoord = 0; // Initial Y coordinate
        newPlayer[0].viewDirection = 0; // Initial direction the PC is facing
        newPlayer[0].health = 3; // Initial Health points
        newPlayer[0].hasShot = 0; // Player has not shot
        newPlayer[0].shotCooldown = 0; // Player has not shot
        
        if(gameState->playerCount==0){
            newPlayer[0].isHost = 1; // First player, mark as host
        }
        else{
            newPlayer[0].isHost = 0;
        }
        
        newPlayer[0].playerNumber = gameState->playerCount; // Player's number
        memcpy(newPlayer[0].playerName, playerName, PLAYERNAME_LENGTH); // Player's name
        newPlayer[0].connectionInfo = connectionInfo; // sockaddr_storage struct        
        gameState->playerList[playerNumber] = newPlayer[0]; // Allocate new player
        
        // Increase number of players
        gameState->playerCount += 1;
    }
    // TODO: Else -> return game full error
  
    return gameState;
}

// Updates the lobby state to every player
void updateLobby(game* gameState, char* buf){
    
    //Set array of array of chars and set values to \0
    char nameArray[4][16];
    memset(nameArray, '\0', 4*16*sizeof(char));
    
    // Write player names to array
    for (int i = 0; i < gameState->playerCount; i++)
    {
        memcpy(nameArray[i], gameState->playerList[i].playerName, PLAYERNAME_LENGTH);   
    }

    //Pack names
    int size = packLobbyState(buf, nameArray[0], nameArray[1], nameArray[2], nameArray[3]);
    setLenout(size);
}

game* addEnemy(game* gameState){

    int randomPlayer = rand() % gameState->playerCount+1; // Choose random player to follow
    int randomXborder = rand() % 2; // Choose random border from x axis
    int randomYborder = rand() % 2; // Choose random border from y axis
    int randomXcoord = rand() % 800; // Choose random border from x axis
    int randomYcoord = rand() % 800; // Choose random border from y axis
    
    enemy newEnemy[1];
    
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

int getPlayerNumber(game* gameState, player_n* connectionInfo){

    int playerNumber = 100;
    for(int i=0;i<gameState->playerCount;i++){
        if(gameState->playerList[i].connectionInfo == connectionInfo){
            playerNumber = gameState->playerList[i].playerNumber;
            break;
        }
    }
    return playerNumber;
}

// Check if cooldown is running
game* checkShotCooldown(game* gameState, int playerNumber){

    struct timeval tempTime = getCurrentTime();
    long currentTime = tempTime.tv_sec + tempTime.tv_usec;

    // Cooldown not running
    if(gameState->playerList[playerNumber].shotCooldown - currentTime <= 0){
        gameState->playerList[playerNumber].shotCooldown = 0;
    // Cooldown running
    } else {
        gameState->playerList[playerNumber].shotCooldown = currentTime;
    } 
    
    return gameState;
}
game* checkSpawnTimer(game* gameState){

    struct timeval tempTime = getCurrentTime();
    long currentTime = tempTime.tv_sec + tempTime.tv_usec;

    // Enemy spawn allowed
    if(gameState->enemySpawnRate - currentTime <= 0){
        gameState->enemySpawnRate = 0;
    // Enemy spawn not allowed
    } else {
        gameState->enemySpawnRate = currentTime;
    } 
    
    return gameState;
}

struct timeval getCurrentTime(){
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime;
}
