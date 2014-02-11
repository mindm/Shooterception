/* Joonas Blomberg 0342152 <joonas.blomberg@lut.fi>
 * packets.c
 *
 *
 * This file has all the packets, packing and unpacking
 */
#include "packets.h"

// Get messages type
uint8_t getmessagetype(char *buf)
{
	uint8_t value = ntohs(*(uint8_t*)&buf[0]);
	return value;
}
// Pack messages type
int packmessagetype(char *buf, uint8_t msgtype)
{
	*(uint8_t*)&buf[0] = htons(msgtype);
	return sizeof(uint8_t);
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
int packAck()
{
    int msgtype = 0 //Whatever is type of ack
    *(uint8_t*)&buf[0] = htons(msgtype);
    return sizeof(uint8_t);
}

//---Client methods---
int packChatMessage(char *message)
{
	uint8_t msgtype = 5; //change
	packmessagetype(buf, msgtype);
	int msg_size = strlen(message);
	memcpy(&buf[1], message, msg_size+1);

	return (sizeof(uint8_t) + msg_size+1);
}

int packCreateGame(char *gameName, int maxPlayers, char *playerName)
{
    uint8_t msgtype = 5; //change
    packmessagetype(buf, msgtype);
    int g_name_size = strlen(gameName);
    int p_name_size = strlen(playerName);
	memcpy(&buf[1], gameName, g_name_size+1);
    memset(&buf[1 + g_name_size + 1], ' ', g_name_size + 1 - 16);
    *(uint16_t*)&buf[17] = htons(maxPlayers);
    memcpy(&buf[18], playerName, p_name_size+1);
    memset(&buf[18 + p_name_size + 1], ' ', p_name_size + 1 - 16);
    
    return (1 + 1 + 16 + 16);
}

int packJoinGame(char *gameName, char *playerName)
{
    uint8_t msgtype = 5; //change
    packmessagetype(buf, msgtype);
    int g_name_size = strlen(gameName);
    int p_name_size = strlen(playerName);
	memcpy(&buf[1], gameName, g_name_size+1);
    memset(&buf[1 + g_name_size + 1], ' ', g_name_size + 1 - 16);
    memcpy(&buf[19], playerName, p_name_size+1);
    memset(&buf[19 + p_name_size + 1], ' ', p_name_size + 1 - 16);
    
    return (1+ 16 + 16);
}

int packStartGame()
{
    uint8_t msgtype = 5; //change
    packmessagetype(buf, msgtype);
    return 1;
}

int packClientState(struct playerInfo, int hasShot, int messageNumber, int timeReply)
{
    uint8_t msgtype = 5; //change
    packmessagetype(buf, msgtype);
    *(uint16_t*)&buf[1] = htons(playerInfo.xcood);
    *(uint16_t*)&buf[3] = htons(playerInfo.ycood);
    *(uint16_t*)&buf[5] = htons(playerInfo.viewDirection);
    *(uint8_t*)&buf[7] = (playerInfo.hasShot);
    *(uint16_t*)&buf[8] = htons(messageNumber);
    *(uint16_t*)&buf[10] = htons(timeReply);
    
    return 12;
}

int packClientExit()
{
    uint8_t msgtype = 5; //change
    packmessagetype(buf, msgtype);
    return 1;
}

/* Unpack*/

void unpackChatMessage(char *message)
{
    memcpy(message, buf+1, 100);
    cutSpace(message);
}


void unpackCreateGame(char *gameName, int *maxPlayers, char *playerName)
{
    memcpy(gameName, buf+1, 16);
    *maxPlayers = ntohs(*(uint8_t*)&buf[17]);
    memcpy(playerName, buf+18, 16);
    
    cutSpace(gameName);
    cutSpace(playerName);
}

void unpackJoinGame(char *gameName, char *playerName)
{
    memcpy(gameName, buf+1, 16);
    memcpy(playerName, buf+18, 16);
        
    cutSpace(gameName);
    cutSpace(playerName);
}
    
void unpackClientState(int *xcood, int *ycood, int *viewDirection,
        bool *hasShot, int *messageNumber, int *timeReply)
{
    /*not ready*/
}
    



/* Server messages */

int packLobbyState(char *player1, char *player2, char *player3, char *player4)
{
    uint8_t msgtype = 5; //change
    packmessagetype(buf, msgtype);
    int p1_len = strlen(player1);
    int p2_len = strlen(player2);
    int p3_len = strlen(player3);
    int p4_len = strlen(player4);
    strcpy(buf+1, player1, p1_len);
    memcpy(buf+1+p1_len, " ", 16 - p1_len);
    strcpy(buf+17, player2, p2_len);
    memcpy(buf+17+p2_len, " ", 16 - p2_len);
    strcpy(buf+33, player3, p3_len);
    memcpy(buf+33+p3_len, " ", 16 - p3_len);
    strcpy(buf+49, player4, p4_len);
    memcpy(buf+49+p4_len, " ", 16 - p4_len);

    return (1 + 16*4);
}

void unpackLobbyState(char *name1, char *name2, char *name3, char *name4)
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


int packerror(char *buf, int code, char *message)
{
	uint16_t msgtype = 1000;
	packmessagetype(buf, msgtype);

	uint8_t x = code;
	*(uint8_t*)&buf[2] = x;

	int msgsize = strlen(message);
	memcpy(&buf[3], &message[0], msgsize);

	int size;
	size = sizeof(uint16_t) + sizeof(uint8_t) + msgsize;

	return size;
}
