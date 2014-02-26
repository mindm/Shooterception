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
	make test_client
	
client:
	$(COMPILER) $(CLIENT_FLAGS) Client/main.c networking/packets.c networking/packets.h -o Client/client $(LIBS)

server:
	$(COMPILER) $(SERVER_FLAGS) gameLogic.c gameLogic.h networking/networking_server.c networking/networking_server.h networking/packets.c networking/packets.h generic.h -lrt -o server

master_server:
	$(COMPILER) $(SERVER_FLAGS) networking/networking_master.c networking/networking_master.h networking/packets.c networking/packets.h generic.h -o master_server

test_client:
	$(COMPILER) $(CLIENT_FLAGS) test_client.c test_client.h networking/packets.c networking/packets.h generic.h -o test_client
	
clean:
	rm client
	rm server
	rm test_client
	rm master_server
	
.PHONY: all client server master_server test_client clean
