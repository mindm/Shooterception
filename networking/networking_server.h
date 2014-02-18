/*
* CT30A5002 Games and Networking 2013-14
* Authors:
* Arttu Hanska (0342181) - arttu.hanska@lut.fi
* Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
* Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#ifndef SERVER_H_
#define SERVER_H_

#define MAXDATASIZE 512
#define MAX_PLAYERS 4

#include "../generic.h"

char *getInAddr(struct sockaddr *sa, char*);
int getInPort(struct sockaddr *sa, int);
void setLenout(int size);
void setSendMask(int mask);


#endif /* SERVER_H_ */
