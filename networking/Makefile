# Joonas Blomberg 0342152 <joonas.blomberg@lut.fi>
# Makefile
FLAGS := -w -std=gnu99
COMPILER := gcc $(FLAGS)
COMPILE := $(COMPILER) -c
LINK := $(COMPILER)
# LINKFLAGS := -lncurses

all: build

build:
	make client
	make server

client: networking_client.o packets.o gameLogic.o
	$(LINK) networking_client.o packets.o gameLogic.o -o client #$(LINKFLAGS)
server: networking_server.o packets.o gameLogic.o
	$(COMPILER) networking_server.o packets.o gameLogic.o -o server
%.o: %.c
	$(COMPILE) $*.c -o $*.o

clean:
	rm *.o
