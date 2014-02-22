COMPILER = gcc
CFLAGS = -w -std=gnu99
DFLAGS = -g
LIBS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -lpthread
SRC = main.c ../networking/packets.c
OUT = main

all:
	$(COMPILER) $(CFLAGS) $(SRC) -o $(OUT) $(LIBS)

debug:
	$(COMPILER) $(CFLAGS) $(DFLAGS) $(SRC) -o $(OUT)

clean:
	rm $(OUT)
