/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#ifndef __MAIN_H

#include <stdio.h>
#include <stdlib.h> 
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
  
#define __MAIN_H

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
 
/* Screen attributes */

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define SCREEN_CX 400
#define SCREEN_CY 400
#define SCREEN_BPP 8

/* Client constants */

#define S_TIME 0.5
#define RANGE 400
#define PC_DIMS 32
#define PC_VEL 10
#define EN_VEL 1
#define FPS 20

#define MAX_EN 20

/* Declarations of graphics, sounds and text objects*/

SDL_Surface *background = NULL;
SDL_Rect bgClip[4];
SDL_Surface *screen = NULL;
SDL_Surface *enemySurf = NULL;
SDL_Surface *fxSurf = NULL;
SDL_Surface *hudSurf = NULL;

Mix_Music *menu_music = NULL;
Mix_Music *game_music = NULL;

Mix_Chunk *pl_bang = NULL;
Mix_Chunk *en_bite = NULL;
Mix_Chunk *en_hit = NULL;

Mix_Chunk *pl_death = NULL;
Mix_Chunk *en_death = NULL;

SDL_Event event;

//number of players/max players in created game
int pl_num_max = 1;
int pl_num = 1;

int gameLevel = 0;

/* Player 1 object */
struct SDLplayer {
	int health;
	int shooting;
	float shoot_time;
	int can_shoot;
	int xVel, yVel;
	int dir;
	int frame;
	int is_attacked;
	SDL_Rect b;
	SDL_Rect animHor[8];
	SDL_Rect animVer[9];
	SDL_Rect fx[6];
	SDL_Surface *playerSurf;
};

//struct player *player = NULL;
struct SDLplayer *players[4];

/* Player 1 shooting line (rectangle which height or width should be 1px) */
SDL_Rect lineRect;

/* Generic enemy object */
struct SDLenemy {
	int health;
	int dir;
	int frame;
	int is_shot;
	SDL_Rect b;
	SDL_Rect animHor[9];
	SDL_Rect animVer[9];
	SDL_Rect fx[2];
};

//struct enemy *enemy = NULL;
struct SDLenemy *enemies[MAX_EN];

/* Function prototypes */
SDL_Surface *loadImage(char*);
void applySurface(int, int, SDL_Surface*, SDL_Surface*, SDL_Rect*);
void drawBackground(int);
void drawHud(struct SDLplayer*);
int init(void);
int loadAssets(void);
void freeAssets(void); 

void setupBgClips(void);
void setupPlayer(void);
void setupEnemy(void);

void handlePlayerInput(int);
void handleShooting(struct SDLplayer*);
void movePlayer(struct SDLplayer*);
void shootBullet(struct SDLplayer*);
void drawPlayer(struct SDLplayer*);
void drawEnemy(struct SDLenemy*);

void handleEnemyDamage(struct SDLenemy*);
void handlePlayerDamage(struct SDLplayer*);

void toggleMenuMusic(void);
void toggleGameMusic(void);

#endif
