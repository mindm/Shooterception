# Joonas Blomberg 0342152 <joonas.blomberg@lut.fi>
# Makefile
FLAGS := -Wall -pedantic -std=gnu99
COMPILER := gcc $(FLAGS)
COMPILE := $(COMPILER) -c
LINK := $(COMPILER)
LINKFLAGS := -lncurses

all: build

build:
	make client
	make server

client: client.o packets.o
	$(LINK) client.o packets.o -o c4client $(LINKFLAGS)
server: server.o packets.o
	$(COMPILER) server.o packets.o -o c4server
%.o: %.c
	$(COMPILE) $*.c -o $*.o

clean:
	rm *.o
