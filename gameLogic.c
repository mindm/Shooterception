/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "gameLogic.h"

int locked = 1;

int startGame(game* gameState, char outbuf[MAXDATASIZE], int levelNumber){

    printf("gameLogic: startGame function\n");
    
    // Change to state inGame
    gameState->currentState = 2;
    
    // Generate random seed for rand()
    srand(time(NULL)); 
  
    // Set level number
    gameState->levelNumber = levelNumber;
        
    // Set level parameters defined by LevelNumber
    gameState = setLevelParameters(gameState);

    // Set initial players locations
    gameState = setPlayerLocations(gameState);

    int size = packGameStart(outbuf,gameState->levelNumber);
    printf("gameLogic: packGameStart called succesfully\n");

    setLenout(size);

    return 0;
}

// Update PC location
game* updatePlayerInfo (game* gameState, player_n* connectionInfo, int xcoord, int ycoord, int viewDirection, int hasShot){

    printf("gameLogic: updatePlayerInfo function\n");
    
    int playerNumber = getPlayerNumber(gameState, connectionInfo);

    printf("gameLogic: playerNumber: %d\n",playerNumber);

    // Check that PC is within game boundaries and update coordinates
    if(xcoord<0){
        gameState->playerList[playerNumber].xcoord = 0;
    }
    else if(xcoord>800){     
        gameState->playerList[playerNumber].xcoord = 800;
    }
    else{
        gameState->playerList[playerNumber].xcoord = xcoord;
    }
    if(ycoord<0){
        gameState->playerList[playerNumber].ycoord = 0;
    }
    else if(ycoord>800){     
        gameState->playerList[playerNumber].ycoord = 800;
    }
    else{
        gameState->playerList[playerNumber].ycoord = ycoord;
    }    

    gameState->playerList[playerNumber].viewDirection = viewDirection;
    gameState->playerList[playerNumber].hasShot = hasShot;

    gameState = checkShotCooldown(gameState, playerNumber);

    // If PC has shot and cooldown timer isn't running, check for hit
    //if(gameState->playerList[playerNumber].hasShot == 1 && gameState->playerList[playerNumber].canShoot == 1){
    if(gameState->playerList[playerNumber].hasShot == 1){
        gameState = checkHit(gameState, playerNumber);   
    }  

    return gameState;
}

// Update enemy location
game* updateEnemyLocations (game* gameState){

    //printf("gameLogic: updateEnemyLocations function\n");
    
    for(int i = 0; i<gameState->enemyCount;i++){

        // Don't move if dead
        if(gameState->enemyList[i].health == 0){
            continue;
        }
        else{
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
    }

    return gameState;
}

// Determine if enemy is shot - if PC has shot check if there are enemies on firing line
game* checkHit (game* gameState, int playerNumber){

    //printf("gameLogic: checkHit function\n");
    
    int k = 0;
    int candidateNumber = 0;
    int hitCandidates[20]; // Used to determine closest enemy
    memset(hitCandidates,0,sizeof(hitCandidates));

    // Check if PC hit enemy
    for(int j=0;j<gameState->enemyCount;j++){
        // Cannot shoot dead enemies
        if(gameState->enemyList[j].health == 0){
            continue;
        } else {
            // PC facing UP - same xcoord, PC has higher ycoord
            if(gameState->playerList[playerNumber].viewDirection == UP){
                if(gameState->playerList[playerNumber].xcoord == gameState->enemyList[j].xcoord && gameState->playerList[playerNumber].ycoord >= gameState->enemyList[j].ycoord){
                    // Enemy in hit line, add to hit candidates
                    hitCandidates[k] = j;
                    k++;
                }
            }
            // PC facing DOWN - same xcoord, PC has lower ycoord           
            else if(gameState->playerList[playerNumber].viewDirection == DOWN){
                if(gameState->playerList[playerNumber].xcoord == gameState->enemyList[j].xcoord && gameState->playerList[playerNumber].ycoord <= gameState->enemyList[j].ycoord){
                    // Enemy in hit line, add to hit candidates
                    hitCandidates[k] = j;
                    k++;
                }                
            }
            // PC facing RIGHT - same ycoord, PC has lower xcoord                
            else if(gameState->playerList[playerNumber].viewDirection == RIGHT){
                if(gameState->playerList[playerNumber].ycoord == gameState->enemyList[j].ycoord && gameState->playerList[playerNumber].xcoord <= gameState->enemyList[j].xcoord){
                    // Enemy in hit line, add to hit candidates
                    hitCandidates[k] = j;
                    k++;
                }                 
            }
            // PC facing LEFT - same ycoord, PC has higher xcoord              
            else if(gameState->playerList[playerNumber].viewDirection == LEFT){
                if(gameState->playerList[playerNumber].ycoord == gameState->enemyList[j].ycoord && gameState->playerList[playerNumber].xcoord >= gameState->enemyList[j].xcoord){
                    // Enemy in hit line, add to hit candidates
                    hitCandidates[k] = j;
                    k++;
                }             
            } 
        }          
    }

    // Something was hit
    printf("k: %d\n\n",k);
    if(k>0){
        // Determine closest enemy in range

        // PC facing UP - closest enemy has highest ycoord 
        if(gameState->playerList[playerNumber].viewDirection == UP){  
            int candidateDistance = 0;
            for(int z=0;z<k;z++){     
                if(gameState->enemyList[hitCandidates[z]].ycoord > candidateDistance){
                    candidateDistance = gameState->enemyList[hitCandidates[z]].ycoord;
                    candidateNumber = hitCandidates[z];
                }
            }
        }
        // PC facing DOWN - closest enemy has lowest ycoord
        else if(gameState->playerList[playerNumber].viewDirection == DOWN){   
            int candidateDistance = 800;
            for(int z=0;z<k;z++){       
                if(gameState->enemyList[hitCandidates[z]].ycoord < candidateDistance){
                    candidateDistance = gameState->enemyList[hitCandidates[z]].ycoord;
                    candidateNumber = hitCandidates[z];                
                }
            }        
        }    
        // PC facing RIGHT - closest enemy has lowest xcoord
        else if(gameState->playerList[playerNumber].viewDirection == RIGHT){    
            int candidateDistance = 800;
            for(int z=0;z<k;z++){      
                if(gameState->enemyList[hitCandidates[z]].xcoord < candidateDistance){
                    candidateDistance = gameState->enemyList[hitCandidates[z]].xcoord;
                    candidateNumber = hitCandidates[z];                
                }
            }    
        }
        // PC facing LEFT - closest enemy has highest xcoord
        else if(gameState->playerList[playerNumber].viewDirection == LEFT){  
            int candidateDistance = 0;
            for(int z=0;z<k;z++){        
                if(gameState->enemyList[hitCandidates[z]].xcoord > candidateDistance){
                    candidateDistance = gameState->enemyList[hitCandidates[z]].xcoord;
                    candidateNumber = hitCandidates[z];                
                }
           }     
        }     

        // Enemy hit, reduce Health points by 1
        if(gameState->enemyList[candidateNumber].health != 0){
            gameState->enemyList[candidateNumber].health -= 1;
        } 

        // Mark the enemy as being hit                 
        gameState->enemyList[candidateNumber].isShot = 1;
                         

        // Check if enemy dies
        if(gameState->enemyList[candidateNumber].health == 1){

            gameState->deadEnemyCount += 1;
            //gameState->enemyCount -= 1;

            /*     
            for(int i = candidateNumber; i < gameState->enemyCount; i++){
                gameState->enemyList[i] = gameState->enemyList[i+1];
            }
            gameState->enemyCount -= 1;
            gameState->enemyLimit -= 1;
            */

        }                             
    }

    return gameState;
}

// Function to determine if player character and enemy collide
game* checkCollision (game* gameState){
    
    //printf("gameLogic: checkCollision function\n");
    
    // Loop through enemies and PCs and check if they're in same coordinate
    for(int i=0;i<gameState->enemyCount;i++){
        for(int j=0;j<gameState->playerCount;j++){
            if(gameState->enemyList[i].xcoord == gameState->playerList[j].xcoord && gameState->enemyList[i].ycoord == gameState->playerList[j].ycoord){
                // Enemy and PC collide, reduce PC's Health points by 1
                gameState->playerList[j].health -= 1;
                // Mark PC as being colliding with enemy
                gameState->playerList[j].isColliding = 1;
                // Check if PC dies
                /*                
                if(gameState->playerList[j].health == 0){
                    gameState = removePlayer(gameState, &gameState->playerList[j].connectionInfo);
                }
                */
            }
        }
    }


    return gameState;
}

// Check end condition: all PCs are dead or enemyLimit and enemyCount are zero
int checkEnd (game* gameState){

    //printf("gameLogic: checkEnd function\n");
    
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

game* initGame(void){
    
    //printf("gameLogic: initGame function\n");
    
    // Allocate game struct
    game *gameState = (game*)malloc(sizeof(game));
    
    memset(gameState->playerList, 0, 4*sizeof(player));
    memset(gameState->enemyList, 0, 20*sizeof(enemy)); 
    gameState->playerCount = 0;
    gameState->enemyCount = 0;
    gameState->deadEnemyCount = 0;
    gameState->levelNumber = 0;
    gameState->enemyLimit = 0;
    gameState->enemySpawnRate = 0;
    gameState->canSpawn = 0;
    gameState->enemyBaseSpeed = 0;
    gameState->currentState = 0;
    gameState->maxPlayers = 0;
    memset(gameState->gameName, 0, sizeof(char));
    gameState->updateTimer = 0;
    gameState->msgNumber = 0;
    
    sendToMaster(*gameState);
    
    return gameState;
}

// New game state
game* newGame(game* gameState, char* gameName, int maxPlayers) {

    //printf("gameLogic: newGame function\n");
    
    // Set the game state
    gameState->playerCount = 0; // No players in new game yet
    gameState->enemyCount = 0; // Number of enemies on the game board
    gameState->levelNumber = 1; // Number of the game level (1-3), new game starts from 1
    strcpy(gameState->gameName, gameName);
    gameState->maxPlayers = maxPlayers;
    
    // Change to inLobby state
    gameState->currentState = 1;

    return gameState;
}

// Free game state
void freeGame(game* gameState) {

    //printf("gameLogic: freeGame function\n");
    
    if(gameState) {
        free(gameState);
    }
}

game* addPlayer(game* gameState, player_n* connectionInfo, char* playerName){

    printf("gameLogic: addPlayer function\n");

    // TODO: Check if packet came from joined client
    
    // Check if player limit is full
    if(gameState->playerCount < gameState->maxPlayers){

    /*
    for(int i=0;i<gameState->playerCount;i++){
        
        if(strcmp(gameState->playerList[i].connectionInfo.address, connectionInfo->address) == 0 && gameState->playerList[i].connectionInfo.port == connectionInfo->port){
            return gameState;
        }
    }
    */
        player newPlayer[1];
        
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
        
        int playerNumber = gameState->playerCount;
        newPlayer[0].playerNumber = playerNumber; // Player's number
        strcpy(newPlayer[0].playerName, playerName); // Player's name     
        newPlayer[0].connectionInfo = *connectionInfo; // sockaddr_storage struct  
        newPlayer[0].isColliding = 0; // New player not colliding with enemies, yet  
        gameState->playerList[playerNumber] = newPlayer[0]; // Allocate new player
        
        gameState->playerCount += 1; // Increase number of players
    }
    
    // TODO: Else -> return game full error
    sendToMaster(*gameState);
    
    return gameState;
}

// Updates the lobby state to every player
void updateLobby(game* gameState, char* buf){
    
	if(locked != 1) {
		//printf("gameLogic: updateLobby function\n");
		
		//Set array of array of chars and set values to \0
		char nameArray[4][17];
		memset(nameArray, '\0', 4*17*sizeof(char));
		
		// Write player names to array
		for (int i = 0; i < gameState->playerCount; i++)
		{   
		    strcpy(nameArray[i], gameState->playerList[i].playerName);   
		}
		
		//Pack names
		int size = packLobbyState(buf, nameArray[0], nameArray[1], nameArray[2], nameArray[3]);
		
		printf("gameLogic: packLobbyState called succesfully\n");
		
		setLenout(size);
	}
	locked = 0;
}

game* addEnemy(game* gameState){

    //printf("gameLogic: addEnemy function\n");

    int randomPlayer = rand() % gameState->playerCount; // Choose random player to follow
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
        newEnemy[0].ycoord = randomYcoord;
        newEnemy[0].viewDirection = RIGHT;
    }        
    newEnemy[0].health = 3; // Initial Health points

    newEnemy[0].following = randomPlayer; // Player who enemy is following
    
    // TODO: Changing individual speed
    newEnemy[0].speed = gameState->enemyBaseSpeed; // Enemy speed
    newEnemy[0].isShot = 0; // New enemy has not been shot, yet
    
    /* 
    int totalEnemies = gameState->enemyCount+gameState->deadEnemyCount;

    // If MAXENEMIES is full replace dead enemy
    if(totalEnemies == MAXENEMIES){
       for(int i=0;i<totalEnemies;i++){
            if(gameState->enemyList[i].health == 0){
                gameState->enemyList[i] = newEnemy[0];
                gameState->deadEnemyCount -= 1;
            }
       }

    } else {
        gameState->enemyList[gameState->enemyCount] = newEnemy[0]; // Allocate new enemy
    }
    */

    gameState->enemyList[gameState->enemyCount] = newEnemy[0]; // Allocate new enemy

    gameState->enemyCount += 1;

    return gameState;
}

game* setLevelParameters(game* gameState){

    //printf("gameLogic: setLevelParameters function\n");
    
    // Set game parameters for game level
    if(gameState->levelNumber == 0){
        gameState->enemyLimit = 10; // How many enemies are spawned
        gameState->enemySpawnRate = 1000000000; // How fast the enemis are spawned, in ns
        //gameState->enemySpawnRate = 1000000; // How fast the enemis are spawned, in ns
        gameState->enemyBaseSpeed = 1; // How fast the enemies _atleast_ move
    } 
    else if(gameState->levelNumber == 1){
        gameState->enemyLimit = 15; // How many enemies are spawned
        gameState->enemySpawnRate = 1000000000; // How fast the enemies are spawned
        gameState->enemyBaseSpeed = 2; // How fast the enemies _atleast_ move
 
    } 
    else if(gameState->levelNumber == 2){
        gameState->enemyLimit = 20; // How many enemies are spawned
        gameState->enemySpawnRate = 1000000000; // How fast the enemies are spawned
        gameState->enemyBaseSpeed = 3; // How fast the enemies _atleast_ move
    }
    else if(gameState->levelNumber == 3){
        gameState->enemyLimit = 20; // How many enemies are spawned
        gameState->enemySpawnRate = 1000000000; // How fast the enemies are spawned
        gameState->enemyBaseSpeed = 3; // How fast the enemies _atleast_ move
    }
    else if(gameState->levelNumber == 4){
        gameState->enemyLimit = 20; // How many enemies are spawned
        gameState->enemySpawnRate = 1000000000; // How fast the enemies are spawned
        gameState->enemyBaseSpeed = 3; // How fast the enemies _atleast_ move
    }
    else if(gameState->levelNumber == 5){
        gameState->enemyLimit = 20; // How many enemies are spawned
        gameState->enemySpawnRate = 1000000000; // How fast the enemies are spawned
        gameState->enemyBaseSpeed = 3; // How fast the enemies _atleast_ move
    }            
    return gameState;
}

game* setPlayerLocations(game* gameState){

    //printf("gameLogic: setPlayerLocations function\n");

    for(int i = 0; i <= gameState->playerCount; i++){
        if(gameState->playerList[i].playerNumber == 0){
            gameState->playerList[i].xcoord = 400;
            gameState->playerList[i].ycoord = 360;            
        }
        else if(gameState->playerList[i].playerNumber == 1){
            gameState->playerList[i].xcoord = 440;
            gameState->playerList[i].ycoord = 400;            
        }
        else if(gameState->playerList[i].playerNumber == 2){
            gameState->playerList[i].xcoord = 400;
            gameState->playerList[i].ycoord = 440;           
        }
        else if(gameState->playerList[i].playerNumber == 3){
            gameState->playerList[i].xcoord = 360;
            gameState->playerList[i].ycoord = 400;           
        }
    
    }
    return gameState;
}

int getPlayerNumber(game* gameState, player_n* connectionInfo){

    //printf("gameLogic: getPlayerNumber function\n");

    int playerNumber = 100;
    for(int i=0;i<gameState->playerCount;i++){
        if(strcmp(gameState->playerList[i].connectionInfo.address, connectionInfo->address) == 0 && gameState->playerList[i].connectionInfo.port == connectionInfo->port){
            playerNumber = gameState->playerList[i].playerNumber;
            break;
        }
    }
    return playerNumber;
}

game* resetEnemyHits(game* gameState){

    //printf("gameLogic: resetEnemyHits function\n");
    
    for(int i=0;i<gameState->enemyCount;i++){
        gameState->enemyList[i].isShot = 0;
    }
    
    return gameState;
}

game* resetPlayerCollisions(game* gameState){

    //printf("gameLogic: resetPlayerCollisions function\n");
    
    for(int i=0;i<gameState->playerCount;i++){
        gameState->playerList[i].isColliding = 0;
    }
    
    return gameState;
}

game* removePlayer(game* gameState, player_n* connectionInfo){

    printf("gameLogic: removePlayer function\n");
    
    for(int i=0;i<gameState->playerCount;i++){
        if(strcmp(gameState->playerList[i].connectionInfo.address, connectionInfo->address) == 0 && gameState->playerList[i].connectionInfo.port == connectionInfo->port){        
            for(int j=i;j<gameState->playerCount;j++){
                gameState->playerList[j] = gameState->playerList[j+1];
            }
            gameState->playerCount -= 1;
        }
    } 

    return gameState;
}

void relayChatMessage(game* gameState,  char outbuf[MAXDATASIZE], char message[CHATMESSAGE_LENGTH]){

    printf("gameLogic: relayChatMessage function\n");

    // Check if private message - sent as "/playerNumber <message>"
    
    if(strncmp(&message[0],"/",1) == 0 || isdigit(message[1]) != 0){
        setSendMask(atoi(&message[1]));
        printf("gameLogic: Relayed private chat message no player %d\n",atoi(&message[1]));
    }
    
    int size = packChatRelay(outbuf, message);
    setLenout(size);
}

game* sendGameState(game* gameState, char outbuf[MAXDATASIZE]){

    //printf("gameLogic: sendGameState function\n");
    
    struct timespec tempTime = getCurrentTime();
    
    unsigned long currentTime = tempTime.tv_sec * 1000000000 + tempTime.tv_nsec;
    //unsigned long currentTime = + tempTime.tv_nsec;
         
    printf("currentTime: %lu\n",currentTime);
    printf("updateTimer: %lu\n",gameState->updateTimer);
    
    // Send gameState every 50ms (= 50000 microseconds = 50000000 nanoseconds)
    if(currentTime - gameState->updateTimer >= 50000000){
        gameState->updateTimer = currentTime;
            
        // TODO: Needs logic to zero the msgNumber when going over int boundary
        int size = packServerState(outbuf, gameState, gameState->msgNumber);
        printf("gameLogic: packServerState called succesfully\n");
        setLenout(size);
        
        gameState->msgNumber += 1;

        // Reset counter to prevent int16 overflow
        if(gameState->msgNumber == 32000){
            gameState->msgNumber = 0;
        }
    }
  
    return gameState;
}

// Check if cooldown is running
game* checkShotCooldown(game* gameState, int playerNumber){

    //printf("gameLogic: checkShotCooldown function\n");

    struct timespec tempTime = getCurrentTime();
    unsigned long currentTime = tempTime.tv_sec * 1000000000 + tempTime.tv_nsec;
    //unsigned long currentTime = tempTime.tv_nsec;
        
    // Cooldown not running
    if(currentTime - gameState->playerList[playerNumber].shotCooldown >= 500000000){
        gameState->playerList[playerNumber].shotCooldown = currentTime;
        gameState->playerList[playerNumber].canShoot = 1;
    // Cooldown running
    } else {
        gameState->playerList[playerNumber].canShoot = 0;
    } 
    
    return gameState;
}

game* checkSpawnTimer(game* gameState){

    //printf("gameLogic: checkSpawnTimer function\n");
    
    //struct timeval tempTime = getCurrentTime();
    struct timespec tempTime = getCurrentTime();    
    unsigned long currentTime = tempTime.tv_sec * 1000000000 + tempTime.tv_nsec;
    //unsigned long currentTime = tempTime.tv_nsec;     
    
    // Enemy spawn allowed
    if(currentTime - gameState->enemySpawnTimer >= gameState->enemySpawnRate){
        gameState->enemySpawnTimer = currentTime;
        gameState->canSpawn = 1;
    // Enemy spawn not allowed
    } else {
        gameState->canSpawn = 0;
    }
    
    return gameState;
}

/*struct timeval getCurrentTime(){

    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime;
} */

struct timespec getCurrentTime(void){

    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    return currentTime;
}

game* nextLevel(game* gameState, char outbuf[MAXDATASIZE]){

		// Send lobby state to clients
		updateLobby(gameState, outbuf); 
		
		gameState->levelNumber += 1;
		
		// Update level parameters
		gameState = setLevelParameters(gameState);
		
		return gameState;
}

game* resetGame(game* gameState, char outbuf[MAXDATASIZE]){

		// Send lobby state to clients
		updateLobby(gameState, outbuf); 
		
		gameState->levelNumber = 0;
		
		// Update level parameters
		gameState = setLevelParameters(gameState);
		
		return gameState;
}
