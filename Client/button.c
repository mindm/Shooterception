/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "SDL/SDL.h"
#include "button.h"

void setupButtons() {

	/* ok prompt */
 	ok_prompt_button.image = loadImage("ok.gif");
	ok_prompt_button.box.x = SCREEN_CX+15;
	ok_prompt_button.box.y = 550;
	ok_prompt_button.box.w = 250;
	ok_prompt_button.box.h = 100;
	ok_prompt_button.action = OK;

	/* cancel prompt*/
 	cancel_prompt_button.image = loadImage("cancel.gif");
	cancel_prompt_button.box.x = SCREEN_CX-215;
	cancel_prompt_button.box.y = 550;
	cancel_prompt_button.box.w = 250;
	cancel_prompt_button.box.h = 100;
	cancel_prompt_button.action = CANCEL;

	/* join game menu */
 	join_menu_button.image = loadImage("join.gif");
	join_menu_button.box.x = SCREEN_CX-125;
	join_menu_button.box.y = 225;
	join_menu_button.box.w = 250;
	join_menu_button.box.h = 100;
	join_menu_button.action = JOIN_MENU;

	/* join selected game button */
 	join_button.image = loadImage("join.gif");
	join_button.box.x = SCREEN_CX-125;
	join_button.box.y = 650;
	join_button.box.w = 250;
	join_button.box.h = 100;
	join_button.action = GAME_LOBBY;


	/* ok selected server in create game menu*/
 	ok_button.image = loadImage("ok.gif");
	ok_button.box.x = SCREEN_CX-125;
	ok_button.box.y = 650;
	ok_button.box.w = 250;
	ok_button.box.h = 100;
	ok_button.action = CREATE_MENU;

	/* change into create game menu */
 	create_menu_button.image = loadImage("create.gif");
	create_menu_button.box.x = SCREEN_CX-125;
	create_menu_button.box.y = 350;
	create_menu_button.box.w = 250;
	create_menu_button.box.h = 100;
	create_menu_button.action = SERVER_MENU;

	/* create game */
 	create_button.image = loadImage("create.gif");
	create_button.box.x = SCREEN_CX-125;
	create_button.box.y = 650;
	create_button.box.w = 250;
	create_button.box.h = 100;
	create_button.action = GAME_LOBBY;

 	quit_menu_button.image = loadImage("quit.gif");
	quit_menu_button.box.x = SCREEN_CX-125;
	quit_menu_button.box.y = 475;
	quit_menu_button.box.w = 250;
	quit_menu_button.box.h = 100;
	quit_menu_button.action = QUIT;

	/* start joined game from game lobby */
 	start_button.image = loadImage("start.gif");
	start_button.box.x = SCREEN_CX-125;
	start_button.box.y = 650;
	start_button.box.w = 250;
	start_button.box.h = 100;
	start_button.action = IN_GAME;

	/* switch buttons for changing the num of players in create game menu*/
 	up_button.image = loadImage("arrow_up.gif");
	up_button.box.x = SCREEN_CX+130;
	up_button.box.y = 268;
	up_button.box.w = 48;
	up_button.box.h = 43;
	up_button.action = INC;

 	down_button.image = loadImage("arrow_down.gif");
	down_button.box.x = SCREEN_CX+130;
	down_button.box.y = 353;
	down_button.box.w = 48;
	down_button.box.h = 43;
	down_button.action = DEC;

	/* text areas */
	server_list_box.x = 200;
	server_list_box.y = 150;
	server_list_box.w = 400;
	server_list_box.h = 480;

	game_name_box.x = 250;
	game_name_box.y = SCREEN_CY+50;
	game_name_box.w = 300;
	game_name_box.h = 64;

	pl_num_box.x = SCREEN_CX+50;
	pl_num_box.y = 308;
	pl_num_box.w = 64;
	pl_num_box.h = 64;

}

/* setup dummy server list */
void setupItemlist(void) {
	int i;
	for(i = 0; i < MAX_ITEMS; i++){
		server_list[i] = malloc(sizeof(struct item));
		server_list[i]->box.x = 240;
		server_list[i]->box.y = 180+32*i;
		server_list[i]->box.w = 350;
		server_list[i]->box.h = 32;
		server_list[i]->id = i;
		server_list[i]->pl_num = 0;
		server_list[i]->pl_num_max = 0;
		server_list[i]->focused = 0;
		strcpy(server_list[i]->name, "default");
	}

	server_list[0]->focused = 1; //first item is focused
}

/* check if button hitbox is clicked and return the appropriate button.action */
int handleButton(struct button _button, int x, int y) {

    if((x > _button.box.x) && (x < _button.box.x + _button.box.w) 
		&& (y > _button.box.y) && (y < _button.box.y + _button.box.h)) {

		return _button.action;	
    }

	return NOTHING;
}

/* check if button hitbox is clicked and return the appropriate button.action */
int handleFocus(struct item *_focus, int x, int y) {

    if((x > _focus->box.x) && (x < _focus->box.x + _focus->box.w) 
		&& (y > _focus->box.y) && (y < _focus->box.y + _focus->box.h)) {
		return _focus->id;	
    }

	return -1;
}

//unfocus all but "id"
void focus(struct item *_focus, int id) {
	if(_focus->id == id) 
		_focus->focused = 1;
	else
		_focus->focused = 0;
}

void showButton(struct button _button) {

    applySurface(_button.box.x, _button.box.y, _button.image, screen, NULL);
}

void freeButtons() {

	SDL_FreeSurface(join_menu_button.image);
	SDL_FreeSurface(create_menu_button.image);
	SDL_FreeSurface(join_button.image);
	SDL_FreeSurface(create_button.image);
	SDL_FreeSurface(quit_menu_button.image);
	SDL_FreeSurface(start_button.image);
	SDL_FreeSurface(up_button.image);
	SDL_FreeSurface(down_button.image);

	SDL_FreeSurface(ok_button.image);
	SDL_FreeSurface(ok_prompt_button.image);
	SDL_FreeSurface(cancel_prompt_button.image);
}

void freeItemlist(void) {
	int i;
	for(i = 0; i < MAX_ITEMS; i++){
		free(server_list[i]);
	}
}

