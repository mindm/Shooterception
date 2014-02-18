all:
	gcc -Wall -g -pedantic -std=gnu99 gameLogic.c gameLogic.h networking/networking_server.c networking/networking_server.h networking/packets.c networking/packets.h generic.h -o game
