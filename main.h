#ifndef __MAIN_H

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
  
#define __MAIN_H

/* Screen attributes */
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int SCREEN_CX = 400;
const int SCREEN_CY = 400;
const int SCREEN_BPP = 8;
const int PC_DIMS = 32;
const int PC_VEL = 10;
const int EN_VEL = 3;
const int FPS = 20;

/* Declarations */
SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *title = NULL;
SDL_Surface *playerSurf = NULL;
SDL_Surface *enemySurf = NULL;

SDL_Event event;

SDL_Rect lineRect;

TTF_Font *font = NULL;
SDL_Color textColor = { 255, 33, 0 };

/* Client constants */
const int IN_GAME = 42;
const int IN_MENU = 21;
const int QUIT = 0;

const int UP = 0; 
const int DOWN = 180; 
const int LEFT = 270; 
const int RIGHT = 90; 

struct player {
	int health;
	int xVel, yVel;
	int shooting;
	int dir;
	int frame;
	SDL_Rect b;
	SDL_Rect animHor[8];
	SDL_Rect animVer[8];
} player;

struct enemy {
	int health;
	int dir;
	int frame;
	SDL_Rect b;
	SDL_Rect animHor[9];
	SDL_Rect animVer[9];
} enemy;


/* Function prototypes */
SDL_Surface *load_image(char*);
void apply_surface(int, int, SDL_Surface*, SDL_Surface*, SDL_Rect*);
void draw_bg(void);
int init(void);
void free_assets(void); 

void set_player(void);
void set_enemy(void);

void handle_input(void);
void move_player(void);
void move_enemies(void);
void shoot_bullet(void);
void draw_player(void);
void draw_enemies(void);

int check_collision(SDL_Rect, SDL_Rect);

#endif
