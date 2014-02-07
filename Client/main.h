/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#ifndef __MAIN_H

#include <stdio.h>
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
#define EN_VEL 3
#define FPS 20

/* Declarations of graphics, sounds and text objects*/

SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *playerSurf = NULL;
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

/* Player 1 object */
struct player {
	int health;
	int xVel, yVel;
	int shooting;
	float shoot_time;
	int dir;
	int frame;
	SDL_Rect b;
	SDL_Rect animHor[8];
	SDL_Rect animVer[8];
	SDL_Rect fx[6];
};

struct player *player = NULL;
//struct player *players[4];

/* Player 1 shooting line (rectangle which height or width should be 1px) */
SDL_Rect lineRect;

/* Generic enemy object */
struct enemy {
	int health;
	int dir;
	int frame;
	SDL_Rect b;
	SDL_Rect animHor[9];
	SDL_Rect animVer[9];
	SDL_Rect fx[2];
};

struct enemy *enemy = NULL;
//struct enemy *enemies[20];

/* Function prototypes */
SDL_Surface *load_image(char*);
void apply_surface(int, int, SDL_Surface*, SDL_Surface*, SDL_Rect*);
void draw_bground(void);
void draw_hud(void);
int init(void);
int load_assets(void);
void free_assets(void); 

void setup_player(void);
void setup_enemy(void);

void handle_player_input(void);
void move_player(void);
void move_enemies(void);
void shoot_bullet(void);
void draw_player(void);
void draw_enemy(struct enemy*);

void handle_enemy_damage(struct enemy*);
void handle_player_damage(struct enemy*);
int check_collision(SDL_Rect, SDL_Rect);

void toggle_menu_music(void);
void toggle_game_music(void);

#endif
