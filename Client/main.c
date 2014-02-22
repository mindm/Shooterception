/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "main.h"
#include "timer.c"
#include "button.c"
#include "input.c"
#include <pthread.h>
#include "../networking/networking_client.c"

/* Main loop */

int main(int argc, char* args[]) { 

	//srand (time(NULL));
	char mem[1] = "m"; //dummy
	int quit = 0;
	
	pthread_t networking;

    playerNames* joined;

	/* actual menu state and temporal menu state */
	int state = MAIN_MENU, _state = MAIN_MENU, previous_state = MAIN_MENU;
	int id = NOTHING, _id = NOTHING;

	//number of players in created game
	int pl_num = 1;

	//for incrementor
	int i = 0;

	if(!init())
		return 1;

	if(!loadAssets())
		return 1;

	setupButtons();
	setupPlayer();
	setupEnemy();
	setupStringInput();
	setupItemlist();
	
	toggleMenuMusic(); // toggle once = play
	Mix_VolumeMusic(100); //max: 128

	//gamestate loop
	while(!quit) {

		/* name entering menu */
		if(state == NAME_MENU) { 
		    if(SDL_PollEvent(&event)) {

				//player name input
				handleStringInput(state, NAME_LEN);

				//if any button hitbox is left-clicked
				if(event.type == SDL_MOUSEBUTTONDOWN) {

				    if(event.type == SDL_QUIT)
				        quit = 1;

					if(event.button.button == SDL_BUTTON_LEFT) {
						if(_state = handleButton(ok_prompt_button, event.button.x, event.button.y)) {
							state = MAIN_MENU;
							resetStringInput();
						}
					}
				}
		    }

		    //bground and title
		    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x66, 0x66, 0x66));
			printTitle(200, 100, "Shooter shooter");

			//game name box with prompt
			printText(game_name_box.x, game_name_box.y-35, "Your name:", textColor);
			SDL_FillRect(screen, &game_name_box, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
			
			//show the name
			printStringInput(game_name_box.x, game_name_box.y+10, &pl_name_str[0], textColor);

        	showButton(ok_prompt_button);

		}
		/* name entering menu ends */
		
		/* main menu */
		if(state == MAIN_MENU) { 
		    if(SDL_PollEvent(&event)) {

				//if any button hitbox is left-clicked
				if(event.type == SDL_MOUSEBUTTONDOWN) {

				    if(event.type == SDL_QUIT)
				        quit = 1;

					if(event.button.button == SDL_BUTTON_LEFT) {
						//temporal _state is used so we don't overwrite the actual state
						if(_state = handleButton(join_menu_button, event.button.x, event.button.y)) {
							state = _state;
						}

						if(_state = handleButton(create_menu_button, event.button.x, event.button.y))
							state = _state;

						if(_state = handleButton(quit_menu_button, event.button.x, event.button.y))
							state = _state;
					}
				}
		    }

		    //bground and title
		    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x66, 0x66, 0x66));
			printTitle(200, 100, "Shooter shooter");

        	showButton(join_menu_button);
			showButton(create_menu_button);
			showButton(quit_menu_button);
		}
		/* main menu end */

		/* join menu */
		if(state == JOIN_MENU) { 
		    if(SDL_PollEvent(&event)) {

				//if any button's hitbox is left-clicked
				if(event.type == SDL_MOUSEBUTTONDOWN) {

				    if(event.type == SDL_QUIT)
				        quit = 1;

					if(event.button.button == SDL_BUTTON_LEFT) {

						int x = event.button.x;
						int y = event.button.y;

						if(_state = handleButton(join_button, x, y))

						{
							state = _state;
							pthread_create(&networking, NULL, networking_thread, NULL);
							SDL_Delay(1000);
							sendJoinGame(id);
                        }
                        
						for(i = 0; i < 3; i++) {
							if(_id = handleFocus(server_list[i], x, y)) {
								id = _id;				
							}
						}

						/* yes, this could be inside the previous loop
						but it doesn't update correctly for some reason. so it has its own loop */
						for(i = 0; i < 3; i++) { focus(server_list[i], id); }
					}
				}
		    }

		    //bground and title
		    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x66, 0x66, 0x66));
			printTitle(300, 100, "Join game");

			//server list box
			SDL_FillRect(screen, &server_list_box, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
			printText(server_list_box.x, server_list_box.y+5, "|Name-------------------|Players|", textColor);
			
			for(i = 0; i < 3; i++) {

				if(server_list[i]->focused)
					SDL_FillRect(screen, &server_list[i]->box, SDL_MapRGB(screen->format, 0x00, 0x66, 0x00));

				printText(server_list[i]->box.x-30, server_list[i]->box.y, itoa(server_list[i]->id, mem), textColor);
				printText(server_list[i]->box.x, server_list[i]->box.y, server_list[i]->name, textColor);
				printText(server_list[i]->box.x+280, server_list[i]->box.y, "5/5", textColor);
			}

        	showButton(join_button);
		}
		/* join menu end */

		/* create game menu */
		if(state == CREATE_MENU) { 
		    if(SDL_PollEvent(&event)) {

				if(event.type == SDL_QUIT)
					quit = 1;

				//game name input
				handleStringInput(state, NAME_LEN);

				//if any button's hitbox is left-clicked
				if(event.type == SDL_MOUSEBUTTONDOWN) {

					if(event.button.button == SDL_BUTTON_LEFT) {

						if(_state = handleButton(up_button, event.button.x, event.button.y)) {
							if(pl_num < 4) pl_num++; //restrict the size <= 4
							else pl_num = 4;
						}

						if(_state = handleButton(down_button, event.button.x, event.button.y)) {
							if(pl_num > 1) pl_num--; //restrict the size >= 1
							else pl_num = 1;
						}

						//there's only one possible state anyway
						if(_state = handleButton(create_button, event.button.x, event.button.y)) {
							state = _state;
							resetStringInput();
						}
					}
				}
		    }

		    //bground and title
		    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x66, 0x66, 0x66));
			printTitle(200, 100, "Create game");
			
			//pl_num box with prompt
			SDL_FillRect(screen, &pl_num_box, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
			printText(275, pl_num_box.y+30, "Max players:", textColor);
			
			printText(475, pl_num_box.y+30, itoa(pl_num, mem), textColor);

			//game name box with prompt
			printText(game_name_box.x, game_name_box.y-35, "Game name:", textColor);
			SDL_FillRect(screen, &game_name_box, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
			
			//show the name
			printStringInput(game_name_box.x, game_name_box.y+10, &g_name_str[0], textColor);
			
			showButton(create_button);
			showButton(up_button);
			showButton(down_button);

		}
		/* create game menu end */

		/* game lobby */
		if(state == GAME_LOBBY) { 
		    if(SDL_PollEvent(&event)) {

			    if(event.type == SDL_QUIT)
			        quit = 1;

				//game name input
				handleStringInput(state, MAX_LEN);

				//if any button hitbox is left-clicked
				if(event.type == SDL_MOUSEBUTTONDOWN) {

					if(event.button.button == SDL_BUTTON_LEFT) {
						if(_state = handleButton(start_button, event.button.x, event.button.y))
							state = _state;

						if(state == IN_GAME) { //start was pressed, transition into game, therefore:
							toggleMenuMusic(); //menu music off
							toggleGameMusic(); //game music on
						}
					}
				}
		    }

		    //bground and title
		    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x66, 0x66, 0x66));
			printTitle(275, 100, "Game lobby");

			//game lobby infobox (using server list box rectangle)
			SDL_FillRect(screen, &server_list_box, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

			printText(225, server_list_box.y+30, "Game name: ", textColor);
			printText(370, server_list_box.y+30, &pl_name_str[0], textColor);
			printText(225, server_list_box.y+60, "Max players: ", textColor);
			printText(400, server_list_box.y+60, itoa(pl_num, mem), textColor);
			printText(225, server_list_box.y+90, "Players joined: ", textColor);


            joined = getPlayers();
            for(i=0; i < joined->playerCount; i++){
                printText(225, server_list_box.y+120+(30*i), &joined->name[i], textColor);
            }

			//show chat input
			printText(225, server_list_box.h+100, "Chat: ", textColor);
			printStringInput(225, server_list_box.h+120, &textbuffer[0], textColor);

			/* CHAT */
			for(i = 0; i < LOGSIZE; i++) {
				if(strlen(&chatlog[i][0]) > 0) 
					printText(225, server_list_box.h+(20*i), &chatlog[i][0], textColor);
			}

			showButton(start_button);
		}
		/* game lobby end */

		/* in game */
		if(state == IN_GAME) {
				initTimer();
			 	while(SDL_PollEvent(&event)) {

					handlePlayerInput(0); //index of controller

					if(event.type == SDL_QUIT)
						quit = 1;
				}

				drawBackground();

				for(i = 0; i < MAX_EN; i++)	{		
					moveEnemy(enemies[i]);
					drawEnemy(enemies[i]);
				}
				
				sendClientState(players[0], 1);
				for(i = 0; i < pl_num; i++)	{
					movePlayer(players[i]);
					drawPlayer(players[i]);

					if(players[i]->shooting) {

						players[i]->shoot_time -= 0.19;

						if(players[i]->shoot_time <= 0.0f) {
							players[i]->shooting = 0; //could be also empty if 
						}
						else {
							shootBullet(players[i]); //just gfx shot

							/*for(i = 0; i < 20; i++)
								handleEnemyDamage(enemies[i]);*/
						}
					}
				}
				
				drawHud(0); // aka foreground

				/*for(i = 0; i < 20; i++)
					handlePlayerDamage(enemies[i]);*/

				//framecap							
				if(deltaTime() < 1000/FPS) {
				    SDL_Delay((1000/FPS) - deltaTime());
				}
			}
			/* in game end */

		if(state == QUIT) { 
			if(SDL_PollEvent(&event)) {

				//player name input
				handleStringInput(state, NAME_LEN);

				//if any button hitbox is left-clicked
				if(event.type == SDL_MOUSEBUTTONDOWN) {
					if(event.button.button == SDL_BUTTON_LEFT) {
						if(_state = handleButton(ok_prompt_button, event.button.x, event.button.y)) {
							quit = 1;
						}

						if(_state = handleButton(cancel_prompt_button, event.button.x, event.button.y)) {
							state = previous_state;
						}
					}
				}
		    }

		    //bground and title
		    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x66, 0x66, 0x66));
			printTitle(100, 100, "Quitting. Are you sure?");

        	showButton(ok_prompt_button);
			showButton(cancel_prompt_button); 
		}

		//update screen
		if(SDL_Flip(screen) == -1)
			return 1;

	} //endwhile

	

    //free all and quit
	freeAssets();

    return 0;
}


/* Functions*/

int init(void) { 

	//SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
        return 0;

    if(!(screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE)))
 		return 0;

    //SDL_mixer
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) {
        return 0;
    }

	SDL_EnableUNICODE(SDL_ENABLE);
	
	//SDL_ttf
	if(TTF_Init() == -1)
		return 0;

    SDL_WM_SetCaption("Shooter shooter: Shooterception, the movie the game", NULL);

	return 1;
}

int loadAssets(void) {

	//load the images
    background = loadImage("grassd.gif");
	enemySurf = loadImage("enemy.gif");
	fxSurf = loadImage("fx.gif");
	hudSurf = loadImage("health.gif");

	//musics
	menu_music = Mix_LoadMUS("sh_menu.ogg");
	game_music = Mix_LoadMUS("sh_game.ogg");

	//sounds
    pl_bang = Mix_LoadWAV("m16.wav");
	en_bite = Mix_LoadWAV("en_bite.wav");
	en_hit = Mix_LoadWAV("en_hit.wav");
	pl_death = Mix_LoadWAV("pl_death.wav");
	en_death = Mix_LoadWAV("en_death.wav");

    //check errors
    if(!(background || enemySurf || fxSurf))
        return 0;

    if(!(pl_bang || en_bite || en_hit || pl_death || en_death))
        return 0;

}

void freeAssets(void) {

	int i;
	for(i = 0; i < MAX_EN; i++) 
		free(enemies[i]);

	for(i = 0; i < 4; i++) {
		SDL_FreeSurface(players[i]->playerSurf);
		free(players[i]);
	}

 	Mix_FreeChunk(pl_bang);
 	Mix_FreeChunk(en_bite);
 	Mix_FreeChunk(en_hit);
 	Mix_FreeChunk(pl_death);
	Mix_FreeChunk(en_death);

	Mix_FreeMusic(menu_music);
	Mix_FreeMusic(game_music);
	
	freeButtons();
	freeItemlist(3);

    SDL_FreeSurface(background);
	SDL_FreeSurface(enemySurf);
	SDL_FreeSurface(fxSurf);
	SDL_FreeSurface(hudSurf);

	Mix_CloseAudio();

	freeFonts();

	SDL_EnableUNICODE(SDL_DISABLE);
	TTF_Quit();
    SDL_Quit();
}

void setupPlayer(void) {

	int i, j;
	int offset;
	for(i = 0; i < 4; i++) {

		if(!players[i])
			players[i] = malloc(sizeof(struct SDLplayer));

		switch(i) {
			case 0: players[i]->playerSurf = loadImage("player1.gif"); break;
			case 1: players[i]->playerSurf = loadImage("player2.gif"); break;
			case 2: players[i]->playerSurf = loadImage("player3.gif"); break;
			case 3: players[i]->playerSurf = loadImage("player4.gif"); break;
		}

		players[i]->frame = 4;
		players[i]->health = 3;
		players[i]->xVel = 0;
		players[i]->yVel = 0;
		players[i]->shooting = 0;
		players[i]->shoot_time = S_TIME;
		players[i]->dir = UP;
	
		/* position */
		players[i]->b.w = PC_DIMS;
		players[i]->b.h = PC_DIMS;
		players[i]->b.x = SCREEN_CX;
		players[i]->b.y = SCREEN_CY;

		/* shooting line collider */
		lineRect.x = players[i]->b.x + PC_DIMS*0.5;
		lineRect.y = players[i]->b.y-RANGE;
		lineRect.w = 1;
		lineRect.h = RANGE;

		/* setup animations */
		offset = 0;
		for(j = 0; j < 8; j++) {
			players[i]->animHor[j].x = offset;
			players[i]->animHor[j].y = 0;
			players[i]->animHor[j].w = PC_DIMS;
			players[i]->animHor[j].h = PC_DIMS;

			offset += PC_DIMS;
		}
	
		offset = 0;
		for(j = 0; j < 8; j++) {
			players[i]->animVer[j].x = offset;
			players[i]->animVer[j].y = PC_DIMS;
			players[i]->animVer[j].w = PC_DIMS;
			players[i]->animVer[j].h = PC_DIMS;

			offset += PC_DIMS;
		}

		offset = 0;
		for(j = 0; j < 6; j++) {
			players[i]->fx[j].x = offset;
			players[i]->fx[j].y = 0;
			players[i]->fx[j].w = 16;
			players[i]->fx[j].h = 16;

			offset += 16;
		}
	}
}
void setupEnemy(void) {

	int i, j;
	int offset;
	for(i = 0; i < MAX_EN; i++) {

	 	enemies[i] = malloc(sizeof(struct SDLenemy));

		enemies[i]->health = 3;
		enemies[i]->dir = DOWN;
		enemies[i]->frame = 0;

		/* position */
		enemies[i]->b.w = PC_DIMS;
		enemies[i]->b.h = PC_DIMS;
		enemies[i]->b.x = 0;//(rand() % 825) - 50;
		enemies[i]->b.y = 0;//(rand() % 825) -50;

		/* setup animations */
		offset = 0; 
		for(j = 0; j < 9; j++) {
			enemies[i]->animHor[j].x = offset;
			enemies[i]->animHor[j].y = 0;
			enemies[i]->animHor[j].w = PC_DIMS;
			enemies[i]->animHor[j].h = PC_DIMS;

			offset += PC_DIMS;
		}
	
		offset = 0;
		for(j = 0; j < 9; j++) {
			enemies[i]->animVer[j].x = offset;
			enemies[i]->animVer[j].y = PC_DIMS;
			enemies[i]->animVer[j].w = PC_DIMS;
			enemies[i]->animVer[j].h = PC_DIMS;

			offset += PC_DIMS;
		}

		offset = 64;
		for(j = 0; j < 2; j++) {
			enemies[i]->fx[j].x = offset;
			enemies[i]->fx[j].y = 0;
			enemies[i]->fx[j].w = 16;
			enemies[i]->fx[j].h = 16;

			offset += 16;
		}
	}
	
}

//handle damage inflicted BY ENEMY TO PLAYER
void handlePlayerDamage(struct SDLenemy *contact) {

	/*if(contact->health <= 0 || player->health <= 0) 
		return;

	if(checkCollision(player->b, contact->b)) {

		//draw blood spatter
		applySurface(player->b.x, player->b.y, fxSurf, screen, &player->fx[4]);

		//decrease health play sounds
		player->health--;
		if(player->health <= 0 ) {
			Mix_PlayChannel(-1, pl_death, 0);
			player->health = 0;
		}
		else	
			Mix_PlayChannel(-1, en_bite, 0);
	}*/
}

//handle damage inflicted BY PLAYER TO ENEMY
void handleEnemyDamage(struct SDLenemy *target) {

	if(target->health <= 0)
		return;

	/*if(checkCollision(lineRect, target->b)) {

		//draw blood spatter
		applySurface(target->b.x, target->b.y, fxSurf, screen, &target->fx[0]);

		//play sounds
		target->health--;
		if(target->health <= 0 ) {
			Mix_PlayChannel(-1, en_death, 0);
			target->health = 0;
		}
		else	
			Mix_PlayChannel(-1, en_hit, 0);
	}*/
}

SDL_Surface *loadImage(char* filename) { 
 
	SDL_Surface* loadedImage = NULL; 
	SDL_Surface* optimizedImage = NULL; 

	loadedImage = IMG_Load(filename); 
 
	if(loadedImage != NULL) { 
		optimizedImage = SDL_DisplayFormat(loadedImage);  
		SDL_FreeSurface(loadedImage);
	}  

	return optimizedImage; 
}

void applySurface(int x, int y, SDL_Surface* src, SDL_Surface* dest, SDL_Rect* clip) {

    SDL_Rect offset;

    offset.x = x;
    offset.y = y;

    SDL_BlitSurface(src, clip, dest, &offset);
}

void drawBackground(void) {

    applySurface(0, 0, background, screen, NULL);
    applySurface(400, 0, background, screen, NULL);
    applySurface(0, 400, background, screen, NULL);
    applySurface(400, 400, background, screen, NULL);
}

void drawHud(int j) {

	int i;	
	for(i = 0; i < players[j]->health; i++)
		applySurface(20+(32*i), 16, hudSurf, screen, NULL);

	/*debug
	for(i = 0; i < enemy->health; i++)
		applySurface(20+(32*i), 750, hudSurf, screen, NULL);*/
}

void handlePlayerInput(int i) {
	if(event.type == SDL_KEYDOWN) {
		switch(event.key.keysym.sym) { 

			case SDLK_UP: 		players[i]->yVel -= PC_VEL; 	break; 
			case SDLK_DOWN: 	players[i]->yVel += PC_VEL; 	break; 
			case SDLK_LEFT: 	players[i]->xVel -= PC_VEL; 	break; 
			case SDLK_RIGHT: 	players[i]->xVel += PC_VEL; 	break;
			case SDLK_w: 		players[i]->dir = UP; 			break; 
			case SDLK_s: 		players[i]->dir = DOWN; 		break; 
			case SDLK_a: 		players[i]->dir = LEFT; 		break; 
			case SDLK_d: 		players[i]->dir = RIGHT; 		break;
			case SDLK_SPACE: 	players[i]->shooting = 1; 		break;

			case SDLK_9:		toggleGameMusic();		break;
			case SDLK_0:		Mix_HaltMusic();			break; 
		}
	}
	else if(event.type == SDL_KEYUP) {
		switch(event.key.keysym.sym) { 

			case SDLK_UP: 		players[i]->yVel += PC_VEL; 		break; 
			case SDLK_DOWN: 	players[i]->yVel -= PC_VEL; 		break; 
			case SDLK_LEFT: 	players[i]->xVel += PC_VEL; 		break; 
			case SDLK_RIGHT: 	players[i]->xVel -= PC_VEL; 		break;
			case SDLK_SPACE:	players[i]->shooting = 0; 
								players[i]->shoot_time = S_TIME;	break; 
		}
	}
}

void movePlayer(struct SDLplayer* _player) {

	if(_player->health <= 0) 
		return;

	//left/right 
	_player->b.x += _player->xVel; 
	
	//screen left/right border 
	if((_player->b.x < 0) || (_player->b.x + PC_DIMS > SCREEN_WIDTH)) { 
		//move back 
		_player->b.x -= _player->xVel; 
	} 

	// up/down 
	_player->b.y += _player->yVel;

	//screen top/bottom border
	if((_player->b.y < 0) || (_player->b.y + PC_DIMS > SCREEN_HEIGHT)) { 
		//move back 
		_player->b.y -= _player->yVel; 
	}
}

void moveEnemy(struct SDLenemy *_enemy) {

	if(_enemy->health <= 0) 
		return;

	/*int x1, y1, x2, y2;
	
	x1 = player->b.x;
	y1 = player->b.y;

	x2 = _enemy->b.x; 
	y2 = _enemy->b.y;

	//calculate x distance to player, move towards
	if((x1 - x2) < 0) {
		_enemy->dir = LEFT;
		_enemy->b.x -= EN_VEL;
	}
	else {
		_enemy->dir = RIGHT;
		_enemy->b.x += EN_VEL;
	}
	//calculate y distance, move towards
	if((y1 - y2) < 0){
		_enemy->dir = UP;
		_enemy->b.y -= EN_VEL;
	}
	else {
		_enemy->dir = DOWN;
		_enemy->b.y += EN_VEL;
	}*/
}

void drawPlayer(struct SDLplayer* _player) {

	if(_player->health <= 0) {
		//draw corpse
		applySurface(_player->b.x, _player->b.y, _player->playerSurf, screen, &_player->animVer[8]);
	}
	else {
		//draw movement
		switch(_player->dir){
			case UP:
				if(_player->frame > 7) _player->frame = 4;
				applySurface(_player->b.x, _player->b.y, _player->playerSurf, screen, &_player->animVer[_player->frame++]);
				break;
			case DOWN:
				if(_player->frame > 3) _player->frame = 0;
				applySurface(_player->b.x, _player->b.y, _player->playerSurf, screen, &_player->animVer[_player->frame++]);
				break;
			case LEFT:
				if(_player->frame > 3) _player->frame = 0;
				applySurface(_player->b.x, _player->b.y, _player->playerSurf, screen, &_player->animHor[_player->frame++]);
				break;
			case RIGHT:
				if(_player->frame > 7) _player->frame = 4;
				applySurface(_player->b.x, _player->b.y, _player->playerSurf, screen, &_player->animHor[_player->frame++]);
				break;
		}
	}
}

void drawEnemy(struct SDLenemy* _enemy) {

	if(_enemy->health <= 0) {
		//draw corpse
		applySurface(_enemy->b.x, _enemy->b.y, enemySurf, screen, &_enemy->animVer[8]);
	}
	else {
		//draw movement
		switch(_enemy->dir){
			case UP:
				if(_enemy->frame > 7) _enemy->frame = 4;
				applySurface(_enemy->b.x, _enemy->b.y, enemySurf, screen, &_enemy->animVer[_enemy->frame++]);
				break;
			case DOWN:
				if(_enemy->frame > 3) _enemy->frame = 0;
				applySurface(_enemy->b.x, _enemy->b.y, enemySurf, screen, &_enemy->animVer[_enemy->frame++]);
				break;
			case LEFT:
				if(_enemy->frame > 3) _enemy->frame = 0;
				applySurface(_enemy->b.x, _enemy->b.y, enemySurf, screen, &_enemy->animHor[_enemy->frame++]);
				break;
			case RIGHT:
				if(_enemy->frame > 7) _enemy->frame = 4;
				applySurface(_enemy->b.x, _enemy->b.y, enemySurf, screen, &_enemy->animHor[_enemy->frame++]);
				break;
		}
	}
}

void shootBullet(struct SDLplayer* _player) {

	if(_player->health <= 0) 
		return;

	//fx position
	int x = 0, y = 0;

	switch(_player->dir){

		case UP:
			lineRect.x = _player->b.x + PC_DIMS*0.5;
			lineRect.y = _player->b.y - RANGE;
			x = _player->b.x + 10;
			y = _player->b.y - 12;
			lineRect.w = 1;
			lineRect.h = RANGE;
			break;

		case DOWN:
			lineRect.x = _player->b.x + PC_DIMS*0.5;
			lineRect.y = _player->b.y + PC_DIMS;
			x = _player->b.x + 8;
			y = _player->b.y + 28;
			lineRect.w = 1;
			lineRect.h = RANGE;
			break;

		case LEFT:
			lineRect.x = _player->b.x - RANGE;
			lineRect.y = _player->b.y + PC_DIMS*0.5;
			x = _player->b.x - 12;
			y = _player->b.y + 8;
			lineRect.w = RANGE;
			lineRect.h = 1;
			break;

		case RIGHT:
			lineRect.x = _player->b.x + PC_DIMS;
			lineRect.y = _player->b.y + PC_DIMS*0.5;
			x = _player->b.x + 28;
			y = _player->b.y + 8;
			lineRect.w = RANGE;
			lineRect.h = 1;
			break;
	}

	//debug
	SDL_FillRect(screen, &lineRect, SDL_MapRGB(screen->format, 255, 255, 255));
	
	applySurface(x, y, fxSurf, screen, &_player->fx[_player->dir]);
	Mix_PlayChannel(-1, pl_bang, 0);
}

int checkCollision(SDL_Rect A, SDL_Rect B) {

    int leftA, rightA, topA, bottomA;
    int leftB, rightB, topB, bottomB;

    //sides of rect A
    leftA = A.x;
    rightA = A.x + A.w;
    topA = A.y;
    bottomA = A.y + A.h;

    //sides of rect B
    leftB = B.x;
    rightB = B.x + B.w;
    topB = B.y;
    bottomB = B.y + B.h;

    //if any of the sides from A are outside of B
    if(bottomA <= topB)
        return 0;

    if(topA >= bottomB)
        return 0;

    if(rightA <= leftB)
        return 0;

    if(leftA >= rightB)
        return 0;

    //if none of the sides from A are outside B
    return 1;
}
void toggleMenuMusic(void) {

	if(!Mix_PlayingMusic())
		Mix_PlayMusic(menu_music, -1);
	else{
		Mix_HaltMusic();
	}
}

void toggleGameMusic(void){
	if(!Mix_PlayingMusic())
		Mix_FadeInMusic(game_music, -1, 1500);
	else {
		if(Mix_PausedMusic())
			Mix_ResumeMusic();
		else 
			Mix_PauseMusic();
	}
}

