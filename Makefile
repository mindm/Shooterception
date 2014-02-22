#CT30A5002 Games and Networking 2013-14
#Authors:
#	Arttu Hanska (0342181) -  arttu.hanska@lut.fi
#	Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
#	Markku Painomaa (0358551) - markku.painomaa@lut.fi

COMPILER := gcc
FLAGS := -Wall -w -g -pedantic -std=gnu99
LIBS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer

all:  build

build:
	make client
	make server
	
client:
	$(COMPILER) $(FLAGS) Client/main.c networking/packets.c networking/packets.h -o client $(LIBS)

server:
	$(COMPILER) $(FLAGS) gameLogic.c gameLogic.h networking/networking_server.c networking/networking_server.h networking/packets.c networking/packets.h generic.h -o server
	
clean:
	rm client
	rm server
	
.PHONY: all client server clean
