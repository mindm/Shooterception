/*
* CT30A5002 Games and Networking 2013-14
* Authors:
* Arttu Hanska (0342181) - arttu.hanska@lut.fi
* Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
* Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "packets.h"

// Get messages type
uint8_t getmessagetype(char *buf)
{
	uint8_t value = (*(uint8_t*)&buf[0]);
	return value;
}
// Pack messages type
int packmessagetype(char *buf, uint8_t msgtype)
{
	*(uint8_t*)&buf[0] = (msgtype);
	return sizeof(uint8_t);
}

int getPlayerCount(char *buf)
{
    uint8_t value = (*(uint8_t*)&buf[1]);
	return value;   
}
int getEnemyCount(char *buf)
{
    uint8_t value = (*(uint8_t*)&buf[2]);
	return value;
}


void cutSpace(char *ptr)
{
    while(*ptr != '\0')
    {
        if (*ptr == ' ')
            *ptr = '\0';
        ptr++;
    }
}


// Pack acknowledgement message
int packAck(char *buf)
{
    int msgtype = ACK;
    packmessagetype(buf, msgtype);
    return sizeof(uint8_t);
}

//---Client methods---
int packChatMessage(char *buf, char *message)
{
	uint8_t msgtype = CHATMESSAGE;
	packmessagetype(buf, msgtype);
	int msg_size = strlen(message);
	memcpy(&buf[1], message, msg_size+1);

	return (sizeof(uint8_t) + msg_size+1);
}

int packCreateGame(char *buf, char *gameName, int maxPlayers, char *playerName)
{
    uint8_t msgtype = CREATEGAME;
    packmessagetype(buf, msgtype);
    int g_name_size = strlen(gameName);
    int p_name_size = strlen(playerName);

	memcpy(&buf[1], gameName, g_name_size+1);
    memset(&buf[1 + g_name_size + 1], ' ', 16 - (g_name_size + 1));
    *(uint8_t*)&buf[17] = (maxPlayers);
    memcpy(&buf[18], playerName, p_name_size+1);
    memset(&buf[18 + p_name_size + 1], ' ', 16 - (p_name_size + 1));
    
    return (1 + 1 + 16 + 16);
}

int packJoinGame(char *buf, char *gameName, char *playerName)
{
    uint8_t msgtype = JOINGAME;
    packmessagetype(buf, msgtype);
    int g_name_size = strlen(gameName);
    int p_name_size = strlen(playerName);
	memcpy(&buf[1], gameName, g_name_size+1);
    memset(&buf[1 + g_name_size + 1], ' ', 16 - (g_name_size + 1));
    memcpy(&buf[17], playerName, p_name_size+1);
    memset(&buf[17 + p_name_size + 1], ' ', 16 - (p_name_size + 1));
    
    return (1+ 16 + 16);
}

int packStartGame(char *buf)
{
    uint8_t msgtype = STARTGAME;
    packmessagetype(buf, msgtype);
    return 1;
}

int packClientState(char *buf, player playerInfo, int messageNumber/*, int timeReply*/)
{
    uint8_t msgtype = CLIENTSTATE;
    packmessagetype(buf, msgtype);
    *(uint16_t*)&buf[1] = htons(playerInfo.xcoord);
    *(uint16_t*)&buf[3] = htons(playerInfo.ycoord);
    *(uint16_t*)&buf[5] = htons(playerInfo.viewDirection);
    *(uint8_t*)&buf[7] = (playerInfo.hasShot);
    *(uint16_t*)&buf[8] = htons(messageNumber);
    //*(uint16_t*)&buf[10] = htons(timeReply);
    
    return 10;
}

int packClientExit(char *buf)
{
    uint8_t msgtype = CLIENTEXIT;
    packmessagetype(buf, msgtype);
    return 1;
}

/* Unpack*/

void unpackChatMessage(char *buf, char *message)
{
    memcpy(message, buf+1, 100);
    //cutSpace(message);
}


void unpackCreateGame(char *buf, char *gameName, int *maxPlayers, char *playerName)
{
    memcpy(gameName, buf+1, 16);
    *maxPlayers = (*(uint8_t*)&buf[17]);
    memcpy(playerName, buf+18, 16);
    
    cutSpace(gameName);
    cutSpace(playerName);
}

void unpackJoinGame(char *buf, char *gameName, char *playerName)
{
    memcpy(gameName, buf+1, 16);
    memcpy(playerName, buf+17, 16);
        
    cutSpace(gameName);
    cutSpace(playerName);
}
    
void unpackClientState(char *buf, player *playerInfo, int *messageNumber/*, int *timeReply*/)
{
    playerInfo->xcoord = ntohs(*(uint16_t*)&buf[1]);
    playerInfo->ycoord = ntohs(*(uint16_t*)&buf[3]);
    playerInfo->viewDirection = ntohs(*(uint16_t*)&buf[5]);
    playerInfo->hasShot = (*(uint8_t*)&buf[7]);
    *messageNumber = ntohs(*(uint16_t*)&buf[8]);
}
    
    

/* Server messages */

int packLobbyState(char *buf, char *player1, char *player2, char *player3, char *player4)
{
    uint8_t msgtype = LOBBYSTATE;
    packmessagetype(buf, msgtype);
    int p1_len = strlen(player1);
    int p2_len = strlen(player2);
    int p3_len = strlen(player3);
    int p4_len = strlen(player4);
    memcpy(buf+1, player1, p1_len);
    memset(buf+1+p1_len, ' ', 16 - p1_len);
    memcpy(buf+17, player2, p2_len);
    memset(buf+17+p2_len, ' ', 16 - p2_len);
    memcpy(buf+33, player3, p3_len);
    memset(buf+33+p3_len, ' ', 16 - p3_len);
    memcpy(buf+49, player4, p4_len);
    memset(buf+49+p4_len, ' ', 16 - p4_len);

    return (1 + 16*4);
}

int packGameStart(char *buf, int gameLevel)
{
    uint8_t msgtype = GAMESTART;
    packmessagetype(buf, msgtype);
    *(uint8_t*)&buf[1] = (gameLevel);
    
    return 2;
}

int packChatRelay(char *buf, char *message)
{
    uint8_t msgtype = CHATRELAY;
    packmessagetype(buf, msgtype);
    int msg_len = strlen(message);
    memcpy(buf+1, message, msg_len+1);
    
    return 2+msg_len;
}

/*
int packServerState(char *buf, int playerCount, int enemyCount,
        int messageNumber, int timeSent)
*/
int packServerState(char *buf, game *gameState, int msgNumber)
{
    uint8_t msgtype = SERVERSTATE;
    packmessagetype(buf, msgtype);
    
    int players = gameState->playerCount;
    int enemies = gameState->enemyCount;
    *(uint8_t*)&buf[1] = (players);
    *(uint8_t*)&buf[2] = (enemies);
    *(uint16_t*)&buf[3] = htons(msgNumber);
    
    int i, cur;
    cur = 5; // cursor for buffer
    
    for (i=0; i < players; i++)
    {   
        *(uint16_t*)&buf[cur] = htons(gameState->playerList[i].xcoord);
        cur += 2;
        *(uint16_t*)&buf[cur] = htons(gameState->playerList[i].ycoord);
        cur += 2;
        *(uint16_t*)&buf[cur] = htons(gameState->playerList[i].viewDirection);
        cur += 2;
        *(uint8_t*)&buf[cur] = (gameState->playerList[i].health);
        cur += 1;
        *(uint8_t*)&buf[cur] = (gameState->playerList[i].hasShot);
        cur += 1;
    }
        for (i=0; i < enemies; i++)
    {   
        *(uint16_t*)&buf[cur] = htons(gameState->enemyList[i].xcoord);
        cur += 2;
        *(uint16_t*)&buf[cur] = htons(gameState->enemyList[i].ycoord);
        cur += 2;
        *(uint16_t*)&buf[cur] = htons(gameState->enemyList[i].viewDirection);
        cur += 2;
        *(uint8_t*)&buf[cur] = (gameState->enemyList[i].health);
        cur += 1;
        *(uint8_t*)&buf[cur] = (gameState->enemyList[i].isShot);
        cur += 1;
    }
    
    //Huffman coding
    int comp_size = Huffman_Compress((unsigned char *)&buf[1], compress_buffer, cur-1);
    memcpy(&buf[1], (char *)compress_buffer, comp_size);
    
    return comp_size+1;
}

// char array length must be 17!
void unpackLobbyState(char *buf, char *name1, char *name2, char *name3, char *name4)
{
    memcpy(name1, buf+1, 16);
    memcpy(name2, buf+17, 16);
    memcpy(name3, buf+33, 16);
    memcpy(name4, buf+49, 16);
    memset(name1+16, '\0', 1);
    memset(name2+16, '\0', 1);
    memset(name3+16, '\0', 1);
    memset(name4+16, '\0', 1);
    
    cutSpace(name1);
    cutSpace(name2);
    cutSpace(name3);
    cutSpace(name4);
    
}

void unpackGameStart(char *buf, int *gameLevel)
{
    *gameLevel = (*(uint8_t*)&buf[1]);
}

void unpackChatRelay(char *buf, char *message, int *msglen)
{
    strcpy(message, buf+1);
    *msglen = strlen(message);
}

void unpackServerState(char *buf, game *gameState, int *msgNumber)
{
    //Decode Huffman
	int msize = 511;
    memcpy(compress_buffer, &buf[1], msize);
    Huffman_Uncompress(compress_buffer, (unsigned char *)&buf[1], msize, msize);
    
    //int comp_size = Huffman_Compress((unsigned char *)&buf[1], compress_buffer, cur-1);
    //memcpy(&buf[1], (char *)compress_buffer, comp_size);
    
    int players = getPlayerCount(buf);
    int enemies = getEnemyCount(buf);
    *msgNumber = ntohs(*(uint16_t*)&buf[3]);
    
    int i, cur;
    cur = 5; // cursor for buffer
    
    for (i=0; i < players; i++)
    {   
        gameState->playerList[i].xcoord = ntohs(*(uint16_t*)&buf[cur]);
        cur += 2;
        gameState->playerList[i].ycoord = ntohs(*(uint16_t*)&buf[cur]);
        cur += 2;
        gameState->playerList[i].viewDirection = ntohs(*(uint16_t*)&buf[cur]);
        cur += 2;
        gameState->playerList[i].health = (*(uint8_t*)&buf[cur]);
        cur += 1;
        gameState->playerList[i].hasShot = (*(uint8_t*)&buf[cur]);
        cur += 1;
    }
        for (i=0; i < enemies; i++)
    {   
        gameState->enemyList[i].xcoord = ntohs(*(uint16_t*)&buf[cur]);
        cur += 2;
        gameState->enemyList[i].ycoord = ntohs(*(uint16_t*)&buf[cur]);
        cur += 2;
        gameState->enemyList[i].viewDirection = ntohs(*(uint16_t*)&buf[cur]);
        cur += 2;
        gameState->enemyList[i].health = (*(uint8_t*)&buf[cur]);
        cur += 1;
        gameState->enemyList[i].isShot = (*(uint8_t*)&buf[cur]);
        cur += 1;
    }
}

int packError(char *buf, int errorCode)
{
    uint8_t msgtype = ERROR;
    packmessagetype(buf, msgtype);
    
    *(uint8_t*)&buf[1] = (errorCode);
    
    return sizeof(uint8_t) * 2;
}

void unpackError(char *buf, int *errorCode)
{    
    
    *errorCode =  *(uint8_t*)&buf[1];
}


/*  Client - Master server  */

int packGamesQuery(char *buf)
{
    uint8_t msgtype = GAMESQUERY;
    packmessagetype(buf, msgtype);
    
    return sizeof(uint8_t) * 1;
}

int packServerQuery(char *buf)
{
    uint8_t msgtype = SERVERQUERY;
    packmessagetype(buf, msgtype);
    
    return sizeof(uint8_t) * 1;
}

int packGameList(char *buf, serverList *list)
{
    uint8_t msgtype = GAMELIST;
    packmessagetype(buf, msgtype);  
    
    int gameCount = 0;
    int count = list->count;
    //*(uint8_t*)&buf[1] = count;
    
    int i, cur;
    cur = 2; // cursor for buffer
    
    for(i=0;i<count;i++)
    {   
        if(list->servers[i].serverState == 1)
        {
            memcpy(buf+cur, list->servers[i].host, 46);
            cur += 46;
            memcpy(buf+cur, list->servers[i].port, 17);
            cur += 17;
            memcpy(buf+cur, list->servers[i].gameName, 17);
            cur += 17;
            *(uint8_t*)&buf[cur] = list->servers[i].maxPlayers;
            cur += 1;
            *(uint8_t*)&buf[cur] = list->servers[i].playerNumber;
            cur += 1;
            gameCount++;
        }
    }   
    *(uint8_t*)&buf[1] = gameCount;
    printf("gc %d\n", gameCount);
    return cur;
}
int packServerList(char *buf, serverList *list)
{
    uint8_t msgtype = SERVERLIST;
    packmessagetype(buf, msgtype);
    
    int serverCount = 0; // count out lobbies
    int count = list->count;
    //*(uint8_t*)&buf[1] = count;
    
    int i, cur;
    cur = 2; // cursor for buffer
    
    for(i=0;i<count;i++)
    {   
        if(list->servers[i].serverState == 0)
        {
        memcpy(buf+cur, list->servers[i].host, 46);
        cur += 46;
        memcpy(buf+cur, list->servers[i].port, 17);
        cur += 17;
        serverCount++;
        }
    }
    *(uint8_t*)&buf[1] = serverCount;
    
    return cur;
}

void unpackGameList(char *buf, serverList *list)
{
    
    int count = *(uint8_t*)&buf[1];
    list->count = count;
    
	printf("%d %d\n", list->count, count);

    int i, cur;
    cur = 2; // cursor for buffer
    
    for(i=0;i<count;i++)
    {   
        
        memcpy(list->servers[i].host ,buf+cur, 46);
        cur += 46;
        memcpy(list->servers[i].port, buf+cur, 17);
        cur += 17;
        memcpy(list->servers[i].gameName, buf+cur, 17);
        cur += 17;
        list->servers[i].maxPlayers = *(uint8_t*)&buf[cur];
        cur += 1;
        list->servers[i].playerNumber = *(uint8_t*)&buf[cur];
        cur += 1;
    }
}


void unpackServerList(char *buf, serverList *list)
{
    
    int count = *(uint8_t*)&buf[1];
    list->count = count;
    
    int i, cur;
    cur = 2; // cursor for buffer
    
    for(i=0;i<count;i++)
    {   
        memcpy(list->servers[i].host ,buf+cur, 46);
        cur += 46;
        memcpy(list->servers[i].port, buf+cur, 17);
        cur += 17;
    }
      
}

/* Master server - game server*/

int packUpdateState(char *buf, game *gameState)
{
    uint8_t msgtype = UPDATESTATE;
    packmessagetype(buf, msgtype);
    
    *(uint8_t*)&buf[1] = gameState->currentState;
    *(uint8_t*)&buf[2] = gameState->playerCount;
    *(uint8_t*)&buf[3] = gameState->maxPlayers;
    memcpy(buf+4, gameState->gameName, 17);
    
    return 21;
}

void unpackUpdateState(char *buf, server *server)
{
    server->serverState = *(uint8_t*)&buf[1];
    server->playerNumber = *(uint8_t*)&buf[2];
    server->maxPlayers = *(uint8_t*)&buf[3];
    memcpy(server->gameName, buf+4, 17);
}

