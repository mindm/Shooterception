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


/* Server messages */

void cutSpace(char *ptr)
{
    while(*ptr != '\0')
    {
        if (*ptr == ' ')
            *ptr = '\0';
        ptr++;
    }
}

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



/* Below are old functions*/
int packcolumn(char *buf, int column)
{
	uint8_t msgtype = 5; 
	packmessagetype(buf, msgtype);
	uint8_t x = column;
	*(uint8_t*)&buf[2] = x;

	return (sizeof(uint16_t) + sizeof(uint8_t));
}

int packjoin(char *buf)
{
	uint16_t msgtype = 0;
	packmessagetype(buf, msgtype);

	return sizeof(uint16_t);
}

int packready(char *buf)
{
	uint16_t msgtype = 2;
	packmessagetype(buf, msgtype);

	return sizeof(uint16_t);
}

int packquit(char *buf)
{
	uint16_t msgtype = 8;
	packmessagetype(buf, msgtype);

	return sizeof(uint16_t);
}

void unpackok(char *buf, int *count, int *id)
{
	*count = (uint8_t)buf[2];
	*id = (uint8_t)buf[3];
}

void unpackstart(char *buf, int *cols, int *rows, int *players)
{
	*cols = (uint8_t)buf[2];
	*rows = (uint8_t)buf[3];
	*players = (uint8_t)buf[4];
}

// char ** passed as char * one dimensional array
// reason: see http://c-faq.com/aryptr/ary2dfunc2.html
void unpackarea(char *buf, char *area, int rows, int cols)
{
	int start = 2;
	int i;
	for (i=rows-1;i>=0;i--) {
		memcpy(&area[i * (rows+1)],&buf[start], cols);
		start += cols+1;
	}
}

void unpackwinner(char *buf, int *winner)
{
	*winner = (uint8_t)buf[2];
}

void unpackerror(char *buf, int *errtype, int numbytes, char* msg)
{
	uint8_t error = (uint8_t)buf[2];
	*errtype = error;

	memcpy(&msg[0], &buf[3], numbytes-3);
	msg[numbytes-3] = '\0';

}


//---Server methods---
int unpackcolumn(char *buf)
{
	uint8_t column = (uint8_t)buf[2];

	return column;
}

int packok(char *buf, int count, int id)
{
	uint16_t msgtype = 1;
	packmessagetype(buf, msgtype);

	uint8_t x = count;
	*(uint8_t*)&buf[2] = x;
	uint8_t y = id;
	*(uint8_t*)&buf[3] = y;

	int size = sizeof(uint16_t) + 2 * sizeof(uint8_t);
	return size;
}

int packstart(char *buf, int cols, int rows, int players)
{
	uint16_t msgtype = 3;
	packmessagetype(buf, msgtype);

	uint8_t x = cols;
	*(uint8_t*)&buf[2] = x;
	uint8_t y = rows;
	*(uint8_t*)&buf[3] = y;
	uint8_t z = players;
	*(uint8_t*)&buf[4] = z;

	int size = sizeof(uint16_t) + 3 * sizeof(uint8_t);
	return size;
}

int packturn(char *buf)
{
	uint16_t msgtype = 4;
	packmessagetype(buf, msgtype);

	return sizeof(uint16_t);
}


int packarea(char *buf, char *area, int rows, int cols)
{
	uint16_t msgtype = 6;
	packmessagetype(buf, msgtype);
	int start = 2;
	int i;
	for (i=rows-1;i>=0;i--) {
		memcpy(&buf[start], &area[i * (rows+1)], cols);
		start += cols;
		buf[start] = '\0';
		start += 1;
	}
	return start;
}


int packwinner(char *buf, int winner)
{
	uint16_t msgtype = 7;
	packmessagetype(buf, msgtype);

	uint8_t x = winner;
	*(uint8_t*)&buf[2] = x;

	int size = sizeof(uint16_t) + sizeof(uint8_t);

	return size;
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
