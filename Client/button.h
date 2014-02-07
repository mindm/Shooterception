/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

/* button actions | game states */
#define IN_GAME 20
#define MAIN_MENU 21
#define JOIN_MENU 22
#define CREATE_MENU 23
#define GAME_LOBBY 24
#define INC 25
#define DEC 26
#define NOTHING 0
#define QUIT 1

struct button {
	SDL_Rect box;
	SDL_Surface *image;
	int action;
};

struct item {
	SDL_Rect box;
	char* name;
	int focused;
	int id;
};

SDL_Rect server_list_box;
SDL_Rect game_name_box;
SDL_Rect pl_num_box;

struct item *server_list[3];

struct button join_menu_button;
struct button create_menu_button;
struct button quit_menu_button;

struct button join_button;
struct button create_button;
struct button start_button;
struct button up_button;
struct button down_button;

void setup_buttons(void);
void setup_itemlist(void);

int handle_button(struct button, int, int);
int handle_focus(struct item*, int, int);
void focus(struct item*, int);

void show_button(struct button);

void free_buttons(void);
