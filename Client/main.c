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

/* Main loop */

int main(int argc, char* args[]) { 

	int quit = 0;

	/* actual menu state and temporal menu state */
	int state = MAIN_MENU, _state = MAIN_MENU;
	int id = NOTHING, _id = NOTHING;

	//number of players in created game
	int pl_num = 1;

	//for incrementor
	int i = 0;

	if(!init())
		return 1;

	if(!load_assets())
		return 1;

	setup_buttons();
	setup_player();
	setup_enemy();

	setup_itemlist();
	
	toggle_menu_music(); // toggle once = play
	Mix_VolumeMusic(100); //max: 128

	//gamestate loop
	while(!quit) {
		
		/* main menu */
		if(state == MAIN_MENU) { 
		    if(SDL_PollEvent(&event)) {

			//if any button's hitbox is left-clicked
			if(event.type == SDL_MOUSEBUTTONDOWN) {
				if(event.button.button == SDL_BUTTON_LEFT) {
					//temporal _state is used so we don't overwrite the actual state
				    if(_state = handle_button(join_menu_button, event.button.x, event.button.y))
						state = _state;

				    if(_state = handle_button(create_menu_button, event.button.x, event.button.y))
						state = _state;

				    if(_state = handle_button(quit_menu_button, event.button.x, event.button.y))
						state = _state;
				}
			}
		        if(event.type == SDL_QUIT)
		            quit = 1;
		    }

		    //bground and title
		    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x66, 0x66, 0x66));
			apply_surface(200, 100, main_title, screen, NULL);

        	show_button(join_menu_button);
			show_button(create_menu_button);
			show_button(quit_menu_button);
		}
		/* main menu end */

		/* join menu */
		if(state == JOIN_MENU) { 
		    if(SDL_PollEvent(&event)) {

				//if any button's hitbox is left-clicked
				if(event.type == SDL_MOUSEBUTTONDOWN) {
					if(event.button.button == SDL_BUTTON_LEFT) {

						int x = event.button.x;
						int y = event.button.y;

						if(_state = handle_button(join_button, x, y))
							state = _state;

						for(i = 0; i < 3; i++) {
							if(_id = handle_focus(server_list[i], x, y)) {
								id = _id;				
							}
						}

						/* yes, this could be inside the previous loop
						but it doesn't update correctly for some reason. so it has its own loop */
						for(i = 0; i < 3; i++) { focus(server_list[i], id); }
					}
				}

				    if(event.type == SDL_QUIT)
				        quit = 1;
		    }

		    //bground and title
		    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x66, 0x66, 0x66));
			apply_surface(300, 100, join_title, screen, NULL);

			//server list box
			SDL_FillRect(screen, &server_list_box, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
			
			
			for(i = 0; i < 3; i++) {

				if(server_list[i]->focused)
					SDL_FillRect(screen, &server_list[i]->box, SDL_MapRGB(screen->format, 0x00, 0x66, 0x00));

				print_text(server_list[i]->box.x, server_list[i]->box.y, server_list[i]->name);
			}

        	show_button(join_button);
		}
		/* join menu end */

		/* create game menu */
		if(state == CREATE_MENU) { 
		    if(SDL_PollEvent(&event)) {

				//if any button's hitbox is left-clicked
				if(event.type == SDL_MOUSEBUTTONDOWN) {
					if(event.button.button == SDL_BUTTON_LEFT) {

						if(_state = handle_button(up_button, event.button.x, event.button.y)) {
							if(pl_num < 4) pl_num++; else pl_num = 4;
						}

						if(_state = handle_button(down_button, event.button.x, event.button.y)) {
							if(pl_num > 1) pl_num--; else pl_num = 1;
						}

						//there's only one state anyway
						if(_state = handle_button(create_button, event.button.x, event.button.y))
							state = _state;
					}
				}

				if(event.type == SDL_QUIT)
					quit = 1;
		    }

		    //bground and title
		    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x66, 0x66, 0x66));
			apply_surface(200, 100, create_title, screen, NULL);
			
			//pl_num box
			SDL_FillRect(screen, &pl_num_box, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
			print_text(275, pl_num_box.y+30, "Max players:");
			
			char mem[1] = "m"; //memory placeholder
			print_text(475, pl_num_box.y+30, itoa(pl_num, mem));

			//game name box
			print_text(game_name_box.x, game_name_box.y-35, "Game name:");
			SDL_FillRect(screen, &game_name_box, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

			show_button(create_button);
			show_button(up_button);
			show_button(down_button);

		}
		/* create game menu end */

		/* game lobby */
		if(state == GAME_LOBBY) { 
		    if(SDL_PollEvent(&event)) {

			//if any button's hitbox is left-clicked
			if(event.type == SDL_MOUSEBUTTONDOWN) {
				if(event.button.button == SDL_BUTTON_LEFT) {
				    state = handle_button(start_button, event.button.x, event.button.y);

					if(state == IN_GAME) { 
						toggle_menu_music(); //menu music off
						toggle_game_music(); //game music on
					}
				}
			}
		        if(event.type == SDL_QUIT)
		            quit = 1;
		    }

		    //bground and title
		    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x66, 0x66, 0x66));
			apply_surface(300, 100, lobby_title, screen, NULL);

			show_button(start_button);
		}
		/* game lobby end */

		/* in game */
		if(state == IN_GAME) {
				init_timer();
			 	while(SDL_PollEvent(&event)) {

					handle_player_input();

					if(event.type == SDL_QUIT)
						quit = 1;
				}

				draw_bground();

				move_player();			
				move_enemies();

				draw_enemies();
				draw_player();
				
				draw_hud(); // aka foreground

				if(player->shooting) {

					player->shoot_time -= 0.19;

					if(player->shoot_time <= 0.0f) {
						player->shooting = 0; //could be also empty if 
					}
					else {
						shoot_bullet(); //just gfx shot
						handle_enemy_damage(enemy); //calculate real shot
					}
				}

				handle_player_damage(enemy);

				//framecap							
				if(delta_time() < 1000/FPS) {
				    SDL_Delay((1000/FPS) - delta_time());
				}
			}
			/* in game end */

		if(state == QUIT) quit = 1;

		//update screen
		if(SDL_Flip(screen) == -1)
			return 1;

	} //endwhile

	

    //free all and quit
	free_assets();

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

	//SDL_ttf
	if(TTF_Init() == -1)
		return 0;

    SDL_WM_SetCaption("Shooter shooter: Shooterception, the movie the game", NULL);

	return 1;
}

int load_assets(void) {

	//load the images
    background = load_image("grassd.gif");
	playerSurf = load_image("player1.gif");
	enemySurf = load_image("enemy.gif");
	fxSurf = load_image("fx.gif");
	hudSurf = load_image("health.gif");

	//texts and fonts
	title_font = TTF_OpenFont("OCR", 36);
	text_font = TTF_OpenFont("OCR", 16);

	main_title = TTF_RenderText_Solid(title_font, "Shooterception", textColor);
	join_title = TTF_RenderText_Solid(title_font, "Join game", textColor);
	lobby_title = TTF_RenderText_Solid(title_font, "Game lobby", textColor);
	create_title = TTF_RenderText_Solid(title_font, "Create a new game", textColor);

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
    if(!(background || playerSurf || enemySurf || fxSurf))
        return 0;

    if(!(pl_bang || en_bite || en_hit || pl_death || en_death))
        return 0;

}

void free_assets(void) {

	free(player);
	free(enemy);

 	Mix_FreeChunk(pl_bang);
 	Mix_FreeChunk(en_bite);
 	Mix_FreeChunk(en_hit);
 	Mix_FreeChunk(pl_death);
	Mix_FreeChunk(en_death);

	Mix_FreeMusic(menu_music);
	Mix_FreeMusic(game_music);
	
	free_buttons();
	free_itemlist(3);

    SDL_FreeSurface(background);
	SDL_FreeSurface(playerSurf);
	SDL_FreeSurface(enemySurf);
	SDL_FreeSurface(fxSurf);
	SDL_FreeSurface(hudSurf);

	SDL_FreeSurface(main_title);
	SDL_FreeSurface(join_title);
	SDL_FreeSurface(create_title);
	SDL_FreeSurface(lobby_title);

	Mix_CloseAudio();

	TTF_Quit();
    SDL_Quit();
}

void setup_player(void) {

	player = malloc(sizeof(struct player));

	player->frame = 4;
	player->health = 3;
	player->xVel = 0;
	player->yVel = 0;
	player->shooting = 0;
	player->shoot_time = S_TIME;
	player->dir = UP;
	
	/* collider box */
	player->b.w = PC_DIMS;
	player->b.h = PC_DIMS;
	player->b.x = SCREEN_CX;
	player->b.y = SCREEN_CY;

	/* shooting line collider */
	lineRect.x = player->b.x + PC_DIMS*0.5;
	lineRect.y = player->b.y-300;
	lineRect.w = 1;
	lineRect.h = 300;

	/* setup animations */
	int offset = 0, i;
	for(i = 0; i < 8; i++) {
		player->animHor[i].x = offset;
		player->animHor[i].y = 0;
		player->animHor[i].w = PC_DIMS;
		player->animHor[i].h = PC_DIMS;

		offset += PC_DIMS;
	}
	
	offset = 0;
	for(i = 0; i < 8; i++) {
		player->animVer[i].x = offset;
		player->animVer[i].y = PC_DIMS;
		player->animVer[i].w = PC_DIMS;
		player->animVer[i].h = PC_DIMS;

		offset += PC_DIMS;
	}

	offset = 0;
	for(i = 0; i < 6; i++) {
		player->fx[i].x = offset;
		player->fx[i].y = 0;
		player->fx[i].w = 16;
		player->fx[i].h = 16;

		offset += 16;
	}
}

void setup_enemy(void) {

 	enemy = malloc(sizeof(struct enemy));

	enemy->health = 3;
	enemy->dir = DOWN;
	enemy->frame = 0;
	enemy->b.w = PC_DIMS;
	enemy->b.h = PC_DIMS;
	enemy->b.x = 0;
	enemy->b.y = 0;

	/* setup animations */
	int offset = 0, i;
	for(i = 0; i < 9; i++) {
		enemy->animHor[i].x = offset;
		enemy->animHor[i].y = 0;
		enemy->animHor[i].w = PC_DIMS;
		enemy->animHor[i].h = PC_DIMS;

		offset += PC_DIMS;
	}
	
	offset = 0;
	for(i = 0; i < 9; i++) {
		enemy->animVer[i].x = offset;
		enemy->animVer[i].y = PC_DIMS;
		enemy->animVer[i].w = PC_DIMS;
		enemy->animVer[i].h = PC_DIMS;

		offset += PC_DIMS;
	}

	offset = 64;
	for(i = 0; i < 2; i++) {
		enemy->fx[i].x = offset;
		enemy->fx[i].y = 0;
		enemy->fx[i].w = 16;
		enemy->fx[i].h = 16;

		offset += 16;
	}
	
}

//handle damage inflicted BY ENEMY TO PLAYER
void handle_player_damage(struct enemy *contact) {

	if(contact->health <= 0) 
		return;

	if(check_collision(player->b, contact->b)) {

		//draw blood spatter
		apply_surface(player->b.x, player->b.y, fxSurf, screen, &player->fx[4]);

		//decrease health play sounds
		player->health--;
		if(player->health <= 0 ) {
			Mix_PlayChannel(-1, pl_death, 0);
			player->health = 0;
		}
		else	
			Mix_PlayChannel(-1, en_bite, 0);
	}
}

//handle damage inflicted BY PLAYER TO ENEMY
void handle_enemy_damage(struct enemy *target) {

	if(target->health <= 0) 
		return;

	if(check_collision(lineRect, target->b)) {

		//draw blood spatter
		apply_surface(target->b.x, target->b.y, fxSurf, screen, &target->fx[0]);

		//play sounds
		target->health--;
		if(target->health <= 0 ) {
			Mix_PlayChannel(-1, en_death, 0);
			target->health = 0;
		}
		else	
			Mix_PlayChannel(-1, en_hit, 0);
	}
}

SDL_Surface *load_image(char* filename) { 
 
	SDL_Surface* loadedImage = NULL; 
	SDL_Surface* optimizedImage = NULL; 

	loadedImage = IMG_Load(filename); 
 
	if(loadedImage != NULL) { 
		optimizedImage = SDL_DisplayFormat(loadedImage);  
		SDL_FreeSurface(loadedImage);
	}  

	return optimizedImage; 
}

void apply_surface(int x, int y, SDL_Surface* src, SDL_Surface* dest, SDL_Rect* clip) {

    SDL_Rect offset;

    offset.x = x;
    offset.y = y;

    SDL_BlitSurface(src, clip, dest, &offset);
}

void draw_bground(void) {

    apply_surface(0, 0, background, screen, NULL);
    apply_surface(400, 0, background, screen, NULL);
    apply_surface(0, 400, background, screen, NULL);
    apply_surface(400, 400, background, screen, NULL);
}

void draw_hud(void) {

	int i;	
	for(i = 0; i < player->health; i++)
		apply_surface(20+(32*i), 16, hudSurf, screen, NULL);

	//turha lopullisessa versiossa
	for(i = 0; i < enemy->health; i++)
		apply_surface(20+(32*i), 750, hudSurf, screen, NULL);
}

void handle_player_input() {
	if(event.type == SDL_KEYDOWN) {
		switch(event.key.keysym.sym) { 

			case SDLK_UP: 		player->yVel -= PC_VEL; 	break; 
			case SDLK_DOWN: 	player->yVel += PC_VEL; 	break; 
			case SDLK_LEFT: 	player->xVel -= PC_VEL; 	break; 
			case SDLK_RIGHT: 	player->xVel += PC_VEL; 	break;
			case SDLK_w: 		player->dir = UP; 			break; 
			case SDLK_s: 		player->dir = DOWN; 		break; 
			case SDLK_a: 		player->dir = LEFT; 		break; 
			case SDLK_d: 		player->dir = RIGHT; 		break;
			case SDLK_SPACE: 	player->shooting = 1; 		break;

			case SDLK_9:		toggle_game_music();		break;
			case SDLK_0:		Mix_HaltMusic();			break; 
		}
	}
	else if(event.type == SDL_KEYUP) {
		switch(event.key.keysym.sym) { 

			case SDLK_UP: 		player->yVel += PC_VEL; 		break; 
			case SDLK_DOWN: 	player->yVel -= PC_VEL; 		break; 
			case SDLK_LEFT: 	player->xVel += PC_VEL; 		break; 
			case SDLK_RIGHT: 	player->xVel -= PC_VEL; 		break;
			case SDLK_SPACE:	player->shooting = 0; 
								player->shoot_time = S_TIME;	break; 
		}
	}
}

void move_player() {

	//left/right 
	player->b.x += player->xVel; 
	
	//screen left/right border 
	if((player->b.x < 0) || (player->b.x + PC_DIMS > SCREEN_WIDTH)) { 
		//move back 
		player->b.x -= player->xVel; 
	} 

	// up/down 
	player->b.y += player->yVel;

	//screen top/bottom border
	if((player->b.y < 0) || ( player->b.y + PC_DIMS > SCREEN_HEIGHT)) { 
		//move back 
		player->b.y -= player->yVel; 
	}
}

void move_enemies() {

	if(enemy->health <= 0) 
		return;

	int x1, y1, x2, y2;
	
	x1 = player->b.x;
	y1 = player->b.y;

	x2 = enemy->b.x; 
	y2 = enemy->b.y;

	//calculate x distance to player, move towards
	if((x1 - x2) < 0) {
		enemy->dir = LEFT;
		enemy->b.x -= EN_VEL;
	}
	else {
		enemy->dir = RIGHT;
		enemy->b.x += EN_VEL;
	}
	//calculate y distance, move towards
	if((y1 - y2) < 0){
		enemy->dir = UP;
		enemy->b.y -= EN_VEL;
	}
	else {
		enemy->dir = DOWN;
		enemy->b.y += EN_VEL;
	}
}

void draw_player() {

	switch(player->dir){
		case UP:
			if(player->frame > 7) player->frame = 4;
			apply_surface(player->b.x, player->b.y, playerSurf, screen, &player->animVer[player->frame++]);
			break;
		case DOWN:
			if(player->frame > 3) player->frame = 0;
			apply_surface(player->b.x, player->b.y, playerSurf, screen, &player->animVer[player->frame++]);
			break;
		case LEFT:
			if(player->frame > 3) player->frame = 0;
			apply_surface(player->b.x, player->b.y, playerSurf, screen, &player->animHor[player->frame++]);
			break;
		case RIGHT:
			if(player->frame > 7) player->frame = 4;
			apply_surface(player->b.x, player->b.y, playerSurf, screen, &player->animHor[player->frame++]);
			break;
	}
}

void draw_enemies() {

	if(enemy->health <= 0) {
		apply_surface(enemy->b.x, enemy->b.y, enemySurf, screen, &enemy->animVer[8]);
	}
	else {
		switch(enemy->dir){
			case UP:
				if(enemy->frame > 7) enemy->frame = 4;
				apply_surface(enemy->b.x, enemy->b.y, enemySurf, screen, &enemy->animVer[enemy->frame++]);
				break;
			case DOWN:
				if(enemy->frame > 3) enemy->frame = 0;
				apply_surface(enemy->b.x, enemy->b.y, enemySurf, screen, &enemy->animVer[enemy->frame++]);
				break;
			case LEFT:
				if(enemy->frame > 3) enemy->frame = 0;
				apply_surface(enemy->b.x, enemy->b.y, enemySurf, screen, &enemy->animHor[enemy->frame++]);
				break;
			case RIGHT:
				if(enemy->frame > 7) enemy->frame = 4;
				apply_surface(enemy->b.x, enemy->b.y, enemySurf, screen, &enemy->animHor[enemy->frame++]);
				break;
		}
	}
}

void shoot_bullet() {

	//fx position
	int x = 0, y = 0;

	switch(player->dir){

		case UP:
			lineRect.x = player->b.x + PC_DIMS*0.5;
			lineRect.y = player->b.y - RANGE;
			x = player->b.x + 10;
			y = player->b.y - 12;
			lineRect.w = 1;
			lineRect.h = RANGE;
			break;

		case DOWN:
			lineRect.x = player->b.x + PC_DIMS*0.5;
			lineRect.y = player->b.y + PC_DIMS;
			x = player->b.x + 8;
			y = player->b.y + 28;
			lineRect.w = 1;
			lineRect.h = RANGE;
			break;

		case LEFT:
			lineRect.x = player->b.x - RANGE;
			lineRect.y = player->b.y + PC_DIMS*0.5;
			x = player->b.x - 12;
			y = player->b.y + 8;
			lineRect.w = RANGE;
			lineRect.h = 1;
			break;

		case RIGHT:
			lineRect.x = player->b.x + PC_DIMS;
			lineRect.y = player->b.y + PC_DIMS*0.5;
			x = player->b.x + 28;
			y = player->b.y + 8;
			lineRect.w = RANGE;
			lineRect.h = 1;
			break;
	}

	//debug
	SDL_FillRect(screen, &lineRect, SDL_MapRGB(screen->format, 255, 255, 255));
	
	apply_surface(x, y, fxSurf, screen, &player->fx[player->dir]);
	Mix_PlayChannel(-1, pl_bang, 0);
}

int check_collision(SDL_Rect A, SDL_Rect B) {

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
void toggle_menu_music(void) {

	if(!Mix_PlayingMusic())
		Mix_PlayMusic(menu_music, -1);
	else{
		Mix_HaltMusic();
	}
}

void toggle_game_music(void){
	if(!Mix_PlayingMusic())
		Mix_FadeInMusic(game_music, -1, 1500);
	else {
		if(Mix_PausedMusic())
			Mix_ResumeMusic();
		else 
			Mix_PauseMusic();
	}
}

