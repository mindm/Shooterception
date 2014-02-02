/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

/* button actions | game states */
#define IN_GAME 25
#define MAIN_MENU 21
#define JOIN_MENU 22
#define CREATE_MENU 23
#define GAME_LOBBY 24
#define NOTHING 0
#define QUIT 1

struct button {
	SDL_Rect box;
	SDL_Surface *image;
	int action;
};

struct button join_menu_button;
struct button create_menu_button;
struct button quit_menu_button;

struct button join_button;
struct button create_button;
struct button start_button;
struct button switch_button;

void setup_buttons(void);

int handle_button(struct button, int, int);

void show_button(struct button);

void free_buttons(void);
