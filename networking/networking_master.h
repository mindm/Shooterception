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

void *get_in_addr(struct sockaddr *sa);
int get_in_port(struct sockaddr *sa);
