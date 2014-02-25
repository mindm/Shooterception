/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

/* button actions | game states */
#define NAME_MENU 18
#define PROMPT_MENU 19
#define IN_GAME 20
#define MAIN_MENU 21
#define JOIN_MENU 22
#define CREATE_MENU 23
#define GAME_LOBBY 24
#define INC 25
#define DEC 26
#define OK 27
#define CANCEL 28
#define SERVER_MENU 29
#define NOTHING 0
#define QUIT 1

/* menu button, can be clicked */
struct button {
	SDL_Rect box;
	SDL_Surface *image;
	int action;
};

/* list item, can be click-focused */
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

struct button ok_prompt_button;
struct button ok_button;
struct button cancel_prompt_button;

struct button join_button;
struct button create_button;
struct button start_button;
struct button up_button;
struct button down_button;

void setupButtons(void);
void setupItemlist(void);

int handleButton(struct button, int, int);
int handleFocus(struct item*, int, int);
void focus(struct item*, int);

void showButton(struct button);

void freeButtons(void);
