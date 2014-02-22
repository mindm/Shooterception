#CT30A5002 Games and Networking 2013-14
#Authors:
#	Arttu Hanska (0342181) -  arttu.hanska@lut.fi
#	Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
#	Markku Painomaa (0358551) - markku.painomaa@lut.fi

FLAGS := -Wall -g -pedantic -std=gnu99
COMPILER := gcc $(FLAGS)
	
all: build
	
build:
	make client
	make server
	
client:
	$(COMPILER) Client/main.c Client/main.h Client/input.c Client/input.h Client/button.c Client/button.h Client/timer.c Client/timer.h networking/networking_client.c networking/networking_client.h networking/packets.c networking/packets.h generic.h -o client
	
server:
	$(COMPILER) gameLogic.c gameLogic.h networking/networking_server.c networking/networking_server.h networking/packets.c networking/packets.h generic.h -o server
	
clean:
	rm *.o

#all:
#	gcc -Wall -g -pedantic -std=gnu99 gameLogic.c gameLogic.h networking/networking_server.c #networking/networking_server.h networking/packets.c networking/packets.h generic.h -o game
