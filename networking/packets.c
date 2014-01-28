/* Joonas Blomberg 0342152 <joonas.blomberg@lut.fi>
 * packets.c
 *
 *
 * This file has all the packets, packing and unpacking
 */
#include "packets.h"

// Get messages type
uint16_t getmessagetype(char *buf)
{
	uint16_t value = ntohs(*(uint16_t*)&buf[0]);
	return value;
}
// Pack messages type
int packmessagetype(char *buf, uint16_t msgtype)
{
	*(uint16_t*)&buf[0] = htons(msgtype);
	return sizeof(uint16_t);
}



//---Client methods---

int packcolumn(char *buf, int column)
{
	uint16_t msgtype = 5;
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
