COMPILER = gcc
CFLAGS = -std=gnu99
DFLAGS = -g
LIBS = -lSDL -lSDL_image -lSDL_ttf
SRC = main.c
OUT = main

all:
	$(COMPILER) $(CFLAGS) $(SRC) -o $(OUT) $(LIBS)

debug:
	$(COMPILER) $(CFLAGS) $(DFLAGS) $(SRC) -o $(OUT)

clean:
	rm $(OUT)
