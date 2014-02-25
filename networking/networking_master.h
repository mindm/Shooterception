/*
* CT30A5002 Games and Networking 2013-14
* Authors:
* Arttu Hanska (0342181) - arttu.hanska@lut.fi
* Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
* Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "packets.h"
#include "../generic.h"

#define MAXDATASIZE 512

char* getInAddr(struct sockaddr *sa, char* ipstr);
int getInPort(struct sockaddr *sa, int p_port);
