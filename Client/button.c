/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "SDL/SDL.h"
#include "button.h"

void setup_buttons() {

	/* change into join game menu */
 	join_menu_button.image = load_image("join.gif");
	join_menu_button.box.x = SCREEN_CX-125;
	join_menu_button.box.y = 225;
	join_menu_button.box.w = 250;
	join_menu_button.box.h = 100;
	join_menu_button.action = JOIN_MENU;

	/*join selected game button*/
 	join_button.image = load_image("join.gif");
	join_button.box.x = SCREEN_CX-125;
	join_button.box.y = 650;
	join_button.box.w = 250;
	join_button.box.h = 100;
	join_button.action = GAME_LOBBY;

	/* change into create game menu */
 	create_menu_button.image = load_image("create.gif");
	create_menu_button.box.x = SCREEN_CX-125;
	create_menu_button.box.y = 350;
	create_menu_button.box.w = 250;
	create_menu_button.box.h = 100;
	create_menu_button.action = CREATE_MENU;

	/* create game */
 	create_button.image = load_image("create.gif");
	create_button.box.x = SCREEN_CX-125;
	create_button.box.y = 650;
	create_button.box.w = 250;
	create_button.box.h = 100;
	create_button.action = CREATE_MENU;

 	quit_menu_button.image = load_image("quit.gif");
	quit_menu_button.box.x = SCREEN_CX-125;
	quit_menu_button.box.y = 475;
	quit_menu_button.box.w = 250;
	quit_menu_button.box.h = 100;
	quit_menu_button.action = QUIT;

	/* start joined game from game lobby */
 	start_button.image = load_image("start.gif");
	start_button.box.x = SCREEN_CX-125;
	start_button.box.y = 650;
	start_button.box.w = 250;
	start_button.box.h = 100;
	start_button.action = IN_GAME;

	/* switch buttons for changing the num of players in create game menu*/
 	switch_button.image = load_image("arrows.gif");
	switch_button.box.x = SCREEN_CX-125;
	switch_button.box.y = 475;
	switch_button.box.w = 48;
	switch_button.box.h = 96;
	switch_button.action = NOTHING;
}

/* check if button hitbox is clicked and return the appropriate button.action */
int handle_button(struct button _button, int x, int y) {

    if((x > _button.box.x) && (x < _button.box.x + _button.box.w) 
		&& (y > _button.box.y) && (y < _button.box.y + _button.box.h)) {

		return _button.action;	
    }

	return NOTHING;
}

void show_button(struct button _button) {

    apply_surface(_button.box.x, _button.box.y, _button.image, screen, NULL);
}

void free_buttons() {


	SDL_FreeSurface(join_menu_button.image);
	SDL_FreeSurface(create_menu_button.image);
	SDL_FreeSurface(join_button.image);
	SDL_FreeSurface(create_button.image);
	SDL_FreeSurface(quit_menu_button.image);
	SDL_FreeSurface(start_button.image);
	SDL_FreeSurface(switch_button.image);
}
