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

	char mem[1] = "m"; //dummy memory holder
	int quit = 0; //while loop breaker
	int sv_count = 0; //server count
	int counter = 0;
	int my_id = 0;
	int i = 0; //common incrementor

	//client networking thread
	pthread_t networking;

    playerNames* joined;

	/* actual menu state and temporal menu state */
	int state = NAME_MENU, _state = NAME_MENU, previous_state = MAIN_MENU;
	int id = NOTHING, _id = NOTHING;



	/* setup all */
	if(!init())
		return 1;

	if(!loadAssets())
		return 1;

	setupButtons();
	setupItemlist();
	setupBgClips();
	setupPlayer();
	setupEnemy();
	setupStringInput();
	
	toggleMenuMusic(); // toggle once = play
	Mix_VolumeMusic(90); //max: 128

	/* GUI loop */
	while(!quit) {

		/* name entering menu */
		if(state == NAME_MENU) { 
		    if(SDL_PollEvent(&event)) {

			    if(event.type == SDL_QUIT)
			        quit = 1;

				//player name input
				handleStringInput(state, NAME_LEN);

				//if any button hitbox is left-clicked
				if(event.type == SDL_MOUSEBUTTONDOWN) {
					if(event.button.button == SDL_BUTTON_LEFT) {
						if(_state = handleButton(ok_prompt_button, event.button.x, event.button.y)) {
							state = MAIN_MENU;
							resetStringInput();

							//start networking
							pthread_create(&networking, NULL, networking_thread, NULL);
							//if needed? SDL_Delay(1000);
						}
					}
				}
		    }

		    //bground and title
		    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x66, 0x66, 0x66));
			printTitle(200, 100, "Hello stranger!");

			//game name box with prompt
			printText(game_name_box.x, game_name_box.y-35, "Your name:", textColor);
			SDL_FillRect(screen, &game_name_box, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
			
			//show the name
			printStringInput(game_name_box.x, game_name_box.y+10, &textbuffer[0], textColor);

        	showButton(ok_prompt_button);

		}
		/* name entering menu ends */
		
		/* main menu */
		if(state == MAIN_MENU) { 
		    if(SDL_PollEvent(&event)) {
			    if(event.type == SDL_QUIT)
			        quit = 1;

				//if any button hitbox is left-clicked
				if(event.type == SDL_MOUSEBUTTONDOWN) {
					if(event.button.button == SDL_BUTTON_LEFT) {
						//temporal _state is used so we don't overwrite the actual state
						if(_state = handleButton(join_menu_button, event.button.x, event.button.y)) {
							state = _state;
							//since we want to join some game, send query for available games
							sendGamesQuery();
						}

						else if(_state = handleButton(create_menu_button, event.button.x, event.button.y)) {
							state = _state;
							//since we want to create game, send query for available game servers
							sendServerQuery();
						}

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
			sendGamesQuery();
			sv_count = getGameList(); 
		    if(SDL_PollEvent(&event)) {

			    if(event.type == SDL_QUIT)
			        quit = 1;

				//if any button's hitbox is left-clicked
				if(event.type == SDL_MOUSEBUTTONDOWN) {
					if(event.button.button == SDL_BUTTON_LEFT) {

						int x = event.button.x;
						int y = event.button.y;

						if(_state = handleButton(join_button, x, y)) {
							
							if(connectServer(id, 1) == 0) {
								SDL_Delay(1000);
								sendJoinGame(id);
								SDL_Delay(1000);
								state = _state;
							}
                        }
                        
						for(i = 0; i < sv_count; i++) {
							if((_id = handleFocus(server_list[i], x, y)) >= 0) {
								id = _id;				
							}
						}

						/* yes, this could be inside the previous loop
						but it doesn't update correctly for some reason. so it has its own loop */
						for(i = 0; i < sv_count; i++) { 
							focus(server_list[i], id); 	
						}
					}
				}
		    }

		    //bground and title
		    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x66, 0x66, 0x66));
			printTitle(300, 100, "Join game");

			//server list box
			SDL_FillRect(screen, &server_list_box, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
			printText(server_list_box.x, server_list_box.y+5, "|Name----------------|Joined|Max|", textColor);
			
			for(i = 0; i < sv_count; i++) {

				if(server_list[i]->focused)
					SDL_FillRect(screen, &server_list[i]->box, SDL_MapRGB(screen->format, 0x00, 0x66, 0x00));

				printText(server_list[i]->box.x-30, server_list[i]->box.y, itoa(server_list[i]->id+1, mem), textColor);
				printText(server_list[i]->box.x, server_list[i]->box.y, server_list[i]->name, textColor);
				printText(server_list[i]->box.x+280, server_list[i]->box.y, itoa(server_list[i]->pl_num, mem), textColor);
				printText(server_list[i]->box.x+300, server_list[i]->box.y, itoa(server_list[i]->pl_num_max, mem), textColor);			
			}

        	showButton(join_button);
		}
		/* join menu end */

		/* Server select for create game */
		if(state == SERVER_MENU) {
			sv_count = getServerList(); 
		    if(SDL_PollEvent(&event)) {

			    if(event.type == SDL_QUIT)
			        quit = 1;

				//if any button's hitbox is left-clicked
				if(event.type == SDL_MOUSEBUTTONDOWN) {
					if(event.button.button == SDL_BUTTON_LEFT) {

						int x = event.button.x;
						int y = event.button.y;

                     	if(_state = handleButton(ok_button, x, y)) {
							
							if(connectServer(id, 0) == 0) {
								SDL_Delay(1000);
								state = _state;
							}
                            						    
                            state = _state;
                        }   

						for(i = 0; i < sv_count; i++) {
							if((_id = handleFocus(server_list[i], x, y)) >= 0) {
								id = _id;				
							}
						}

						/* yes, this could be inside the previous loop
						but it doesn't update correctly for some reason. so it has its own loop */
						for(i = 0; i < sv_count; i++) { 
							focus(server_list[i], id); 
						}
					}
				}
		    }

		    //bground and title
		    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x66, 0x66, 0x66));
			printTitle(225, 100, "Select server");

			//server list box
			SDL_FillRect(screen, &server_list_box, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
			printText(server_list_box.x, server_list_box.y+5, "|Name---------------------------|", textColor);
			
			for(i = 0; i < sv_count; i++) {

				if(server_list[i]->focused)
					SDL_FillRect(screen, &server_list[i]->box, SDL_MapRGB(screen->format, 0x00, 0x66, 0x00));

				printText(server_list[i]->box.x-30, server_list[i]->box.y, itoa(server_list[i]->id+1, mem), textColor);
				printText(server_list[i]->box.x, server_list[i]->box.y, server_list[i]->name, textColor);
			}

        	showButton(ok_button);
		}
		/* Server select for create game ends */

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
							if(pl_num_max < 4) pl_num_max++; //restrict the size <= 4
							else pl_num_max = 4;
						}

						if(_state = handleButton(down_button, event.button.x, event.button.y)) {
							if(pl_num_max > 1) pl_num_max--; //restrict the size >= 1
							else pl_num_max = 1;
						}

						//there's only one possible state anyway
						if(_state = handleButton(create_button, event.button.x, event.button.y)) {
							state = _state;
							sendCreateGame();
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
			
			printText(475, pl_num_box.y+30, itoa(pl_num_max, mem), textColor);

			//game name box with prompt
			printText(game_name_box.x, game_name_box.y-35, "Game name:", textColor);
			SDL_FillRect(screen, &game_name_box, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
			
			//show the name
			printStringInput(game_name_box.x, game_name_box.y+10, &textbuffer[0], textColor);
			
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
						if(_state = handleButton(start_button, event.button.x, event.button.y)) {
							state = _state;
							sendGameStart();
                            SDL_Delay(1000);
						}

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

			//pl_num_max = getMaxPlayers();
			printText(225, server_list_box.y+30, "Game name: ", textColor);
			printText(370, server_list_box.y+30, &g_name_str[0], textColor);
			printText(225, server_list_box.y+60, "Max players: ", textColor);
			printText(400, server_list_box.y+60, itoa(pl_num_max, mem), textColor);
			printText(225, server_list_box.y+90, "Players joined: ", textColor);

            joined = getPlayers();
			pl_num = joined->playerCount;
            for(i=0; i < pl_num; i++){
                printText(225, server_list_box.y+120+(30*i), joined->name[i], textColor);
            }

			for(i = 0; i < pl_num; i++){
				if(strcmp(joined->name[i], pl_name_str) == 0) {
					my_id = i;
					break;
				}
			}

			//show chat input
			printText(225, server_list_box.h+100, "Chat: ", textColor);
			printStringInput(225, server_list_box.h+120, &textbuffer[0], textColor);

			/* CHAT */
			for(i = 0; i < LOGSIZE; i++) {
				if(strlen(&chatlog[i][0]) > 0) 
					printText(225, server_list_box.h+(20*i), &chatlog[i][0], chatColor);
			}

			if(strcmp(joined->name[0], pl_name_str) == 0) {
				showButton(start_button);
			}
			else if(getGameStart()) {
				state = IN_GAME;
			}
		}
		/* game lobby end */

		/* in game */
		if(state == IN_GAME) {
				initTimer();
			 	while(SDL_PollEvent(&event)) {

					if(event.type == SDL_QUIT){
						quit = 1; //state == QUIT;
					}

					handlePlayerInput(my_id); //index of controller
				}

				drawBackground(gameLevel); //level n:o here 0-3

				for(i = 0; i < MAX_EN; i++)	{		
					updateEnemyStates(enemies[i], i);
					drawEnemy(enemies[i]);
					handleEnemyDamage(enemies[i]);
				}

				movePlayer(players[my_id]);
				drawPlayer(players[my_id]);
				handleShooting(players[my_id]);

				for(i = 0; i < pl_num; i++)	{

					if(my_id == i)
						continue;

					updatePlayerStates(players[i], i);
					drawPlayer(players[i]);
					handlePlayerDamage(players[i]);
					if(players[i]->shooting) 
						shootBullet(players[i]);
					//handleShooting(players[i]);
				}

				sendClientState(players[my_id], counter++); //send player movements to server
				
				if(counter >= 65530) counter = 0;

				drawHud(0); // aka foreground

				//framecap							
				if(deltaTime() < 1000/FPS) {
				    SDL_Delay((1000/FPS) - deltaTime());
				}
			}
			/* in game end */

		if(state == QUIT) { 
			if(SDL_PollEvent(&event)) {

				//if any button hitbox is left-clicked
				if(event.type == SDL_MOUSEBUTTONDOWN) {
					if(event.button.button == SDL_BUTTON_LEFT) {
						if(_state = handleButton(ok_prompt_button, event.button.x, event.button.y)) {
							//sendClientQuit();
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

		if(quit) {
			sendClientQuit();
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
    background = loadImage("bg.gif");
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
	freeItemlist();

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

void setupBgClips(void) {
	int i;
	for(i = 0; i < 4; i++) {
		bgClip[i].w = 400;
		bgClip[i].h = 400;
	}

	bgClip[0].x = 0;
	bgClip[0].y = 0;

	bgClip[1].x = 400;
	bgClip[1].y = 0;

	bgClip[2].x = 0;
	bgClip[2].y = 400;

	bgClip[3].x = 400;
	bgClip[3].y = 400;

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

		players[i]->xVel = 0;
		players[i]->yVel = 0;
		players[i]->can_shoot = 1;
		players[i]->frame = 4;
		players[i]->health = 3;
		players[i]->shooting = 0;
		players[i]->shoot_time = S_TIME;
		players[i]->is_attacked = 0;
		players[i]->dir = UP;
	
		/* position */
		players[i]->b.w = PC_DIMS;
		players[i]->b.h = PC_DIMS;
		players[i]->b.x = SCREEN_CX;
		players[i]->b.y = SCREEN_CY;

		/* shooting line collider */
		lineRect.x = players[i]->b.x + PC_DIMS*0.5;
		lineRect.y = players[i]->b.y - RANGE;
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
		for(j = 0; j < 7; j++) {
			players[i]->animVer[j].x = offset;
			players[i]->animVer[j].y = PC_DIMS;
			players[i]->animVer[j].w = PC_DIMS;
			players[i]->animVer[j].h = PC_DIMS;

			offset += PC_DIMS;
		}

		/* KO */
		players[i]->animVer[8].x = offset+PC_DIMS;
		players[i]->animVer[8].y = 0;
		players[i]->animVer[8].w = PC_DIMS;
		players[i]->animVer[8].h = PC_DIMS+PC_DIMS;

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
		enemies[i]->is_shot = 0;
		enemies[i]->frame = 0;

		/* position */
		enemies[i]->b.w = PC_DIMS;
		enemies[i]->b.h = PC_DIMS;
		enemies[i]->b.x = 0;
		enemies[i]->b.y = 0;

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

//draw blood spatter on player
void handlePlayerDamage(struct SDLplayer *_player) {

	if(_player->health <= 0) 
		return;

	if(_player->is_attacked) {

		//draw blood spatter
		applySurface(_player->b.x, _player->b.y, fxSurf, screen, &_player->fx[4]);

		//decrease health play sounds
		if(_player->health <= 0 ) {
			Mix_PlayChannel(-1, pl_death, 0);
			_player->health = 0;
		}
		else	
			Mix_PlayChannel(-1, en_bite, 0);
	}
}

//draw blood spatter on enemy
void handleEnemyDamage(struct SDLenemy *_enemy) {

	if(_enemy->health <= 0)
		return;

	if(_enemy->is_shot) {

		//draw blood spatter
		applySurface(_enemy->b.x, _enemy->b.y, fxSurf, screen, &_enemy->fx[0]);

		//play sounds
		if(_enemy->health <= 0 ) {
			Mix_PlayChannel(-1, en_death, 0);
			_enemy->health = 0;
		}
		else	
			Mix_PlayChannel(-1, en_hit, 0);
	}
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

void drawBackground(int level) {

    applySurface(0, 0, background, screen, &bgClip[level]);
    applySurface(400, 0, background, screen, &bgClip[level]);
    applySurface(0, 400, background, screen, &bgClip[level]);
    applySurface(400, 400, background, screen, &bgClip[level]);
}

void drawHud(int index) {
	int i;	
	for(i = 0; i < players[index]->health; i++)
		applySurface(20+(32*i), 16, hudSurf, screen, NULL);
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
	
	_player->b.x += _player->xVel;

    if((_player->b.x < 0) || (_player->b.x + PC_DIMS > SCREEN_WIDTH)) {
        _player->b.x -= _player->xVel;
    }

	_player->b.y += _player->yVel;

    if((_player->b.y < 0) || (_player->b.y + PC_DIMS > SCREEN_HEIGHT)) {
		_player->b.y -= _player->yVel;
    }			

}

void handleShooting(struct SDLplayer* _player) { 
	
	if(!_player->can_shoot || !_player->shooting)
		return;


	if(_player->shooting) {

		_player->shoot_time -= 0.19;

		if(!_player->can_shoot || _player->shoot_time <= 0.0f) {
			_player->shooting = 0;
		}
		else {
			shootBullet(_player); //just gfx shot
		}
	}

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

