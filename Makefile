#CT30A5002 Games and Networking 2013-14
#Authors:
#	Arttu Hanska (0342181) -  arttu.hanska@lut.fi
#	Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
#	Markku Painomaa (0358551) - markku.painomaa@lut.fi

COMPILER := gcc
FLAGS := -Wall -w -g -pedantic -std=gnu99
CLIENT_FLAGS := -Wall -w -g -pedantic -std=gnu99
SERVER_FLAGS := -Wall -g -pedantic -std=gnu99

LIBS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -lpthread

all:  build

build:
	make client
	make server
	make master
	
client:
	$(COMPILER) $(CLIENT_FLAGS) Client/main.c networking/packets.c networking/packets.h networking/huffman.c -o Client/client $(LIBS)

server:
	$(COMPILER) $(SERVER_FLAGS) gameLogic.c gameLogic.h networking/networking_server.c networking/networking_server.h networking/packets.c networking/packets.h generic.h networking/huffman.c -lrt -lm -o server

master:
	$(COMPILER) $(SERVER_FLAGS) networking/networking_master.c networking/packets.c networking/huffman.c -o master
	
clean:
	rm client
	rm server
	rm master
	
.PHONY: all client server master clean
