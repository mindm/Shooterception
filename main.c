#include "main.h"
#include "timer.c"
#include "button.c"

/* Main loop */

int main(int argc, char* args[]) { 

	int quit = 0;
	int state = IN_MENU;

	if(!init())
		return 1;

	font = TTF_OpenFont("OCR", 36);
	title = TTF_RenderText_Solid(font, "Shooterception", textColor);
    
	//load the images
    background = load_image("grassd.gif");
	playerSurf = load_image("player1.gif");
	enemySurf = load_image("enemy.gif");

	set_buttons();
	set_player();
	set_enemy();

	//wait until quitting
	while(!quit) {
		
		/* main menu */
		if(state == IN_MENU) { 
		    if(SDL_PollEvent(&event)){
		        state = handle_buttons();
		        if(event.type == SDL_QUIT)
		            quit = 1;
		    }

		    //bg and title
		    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x66, 0x66, 0x66));
			apply_surface(200, 100, title, screen, NULL);

        	show_buttons();

			if(SDL_Flip(screen) == -1)
				return 1;
		}

		/* main menu end */
		/* in game */
		if(state == IN_GAME) {
				startTimer();
			 	while(SDL_PollEvent(&event)) {

					handle_input();

					if(event.type == SDL_QUIT)
						quit = 1;
				}

				draw_bg();

				move_player();			
				move_enemies();

				draw_enemies();
				draw_player();

				if(player.shooting) {
					shoot_bullet();

					if(check_collision(lineRect, enemy.b)){
						enemy.health--;
					}
				}

				//update
				if(SDL_Flip(screen) == -1)
					return 1;

				//framecap							
				if(get_ticks() < 1000/FPS) {
				    SDL_Delay((1000/FPS) - get_ticks());
				}
			}
			/* in game end */

		if(state == QUIT) quit = 1;

	} //endwhile

	

    //Free the surfaces and quit
	free_assets();
	TTF_Quit();
    SDL_Quit();

    return 0;
}


/* Functions*/

int init(void) { 

    if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
        return 0;

    if(!(screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE)))
 		return 0;

	if(TTF_Init() == -1)
		return 0;

    SDL_WM_SetCaption("Main", NULL);

	return 1;
}

void free_assets(void) {

	TTF_CloseFont(font);

	free_buttons();
    SDL_FreeSurface(background);
	SDL_FreeSurface(playerSurf);
	SDL_FreeSurface(enemySurf);
	SDL_FreeSurface(title);
}

void set_player(void) {

	player.frame = 4;
	player.health = 3;
	player.shooting = 0;
	player.dir = UP;
	player.b.w = PC_DIMS;
	player.b.h = PC_DIMS;
	player.b.x = SCREEN_CX;
	player.b.y = SCREEN_CY;

	lineRect.x = player.b.x + PC_DIMS*0.5;
	lineRect.y = player.b.y-300;
	lineRect.w = 1;
	lineRect.h = 300;

	int offset = 0, i;
	for(i = 0; i < 8; i++) {
		player.animHor[i].x = offset;
		player.animHor[i].y = 0;
		player.animHor[i].w = PC_DIMS;
		player.animHor[i].h = PC_DIMS;

		offset += PC_DIMS;
	}
	
	offset = 0;
	for(i = 0; i < 8; i++) {
		player.animVer[i].x = offset;
		player.animVer[i].y = PC_DIMS;
		player.animVer[i].w = PC_DIMS;
		player.animVer[i].h = PC_DIMS;

		offset += PC_DIMS;
	}
}

void set_enemy(void) {
	enemy.health = 3;
	enemy.dir = DOWN;
	enemy.frame = 0;
	enemy.b.w = PC_DIMS;
	enemy.b.h = PC_DIMS;
	enemy.b.x = 0;
	enemy.b.y = 0;

	int offset = 0, i;
	for(i = 0; i < 9; i++) {
		enemy.animHor[i].x = offset;
		enemy.animHor[i].y = 0;
		enemy.animHor[i].w = PC_DIMS;
		enemy.animHor[i].h = PC_DIMS;

		offset += PC_DIMS;
	}
	
	offset = 0;
	for(i = 0; i < 9; i++) {
		enemy.animVer[i].x = offset;
		enemy.animVer[i].y = PC_DIMS;
		enemy.animVer[i].w = PC_DIMS;
		enemy.animVer[i].h = PC_DIMS;

		offset += PC_DIMS;
	}

}

SDL_Surface *load_image(char* filename) { 
 
	SDL_Surface* loadedImage = NULL; 
	SDL_Surface* optimizedImage = NULL; 

	loadedImage = IMG_Load(filename); 
 
	if(loadedImage != NULL) { 
		optimizedImage = SDL_DisplayFormat(loadedImage);  
		SDL_FreeSurface(loadedImage);

		/*if(optimizedImage != NULL){
			Uint32 colorkey = SDL_MapRGB(optimizedImage->format, 0xFF, 0xA0, 0xB4);
			SDL_SetColorKey(optimizedImage, SDL_SRCCOLORKEY, colorkey);
		} */
	}  

	return optimizedImage; 
}

void apply_surface(int x, int y, SDL_Surface* src, SDL_Surface* dest, SDL_Rect* clip) {

    SDL_Rect offset;

    offset.x = x;
    offset.y = y;

    SDL_BlitSurface(src, clip, dest, &offset);
}

void draw_bg(void) {
    apply_surface(0, 0, background, screen, NULL);
    apply_surface(400, 0, background, screen, NULL);
    apply_surface(0, 400, background, screen, NULL);
    apply_surface(400, 400, background, screen, NULL);
}

void handle_input() {
	if(event.type == SDL_KEYDOWN) {
		switch(event.key.keysym.sym) { 

			case SDLK_UP: 
				player.yVel -= PC_VEL;
				break; 
			case SDLK_DOWN: 
				player.yVel += PC_VEL;
				break; 
			case SDLK_LEFT: 
				player.xVel -= PC_VEL; 
				break; 
			case SDLK_RIGHT: 
				player.xVel += PC_VEL; 
				break;
			case SDLK_w: 
				player.dir = UP; 
				break; 
			case SDLK_s: 
				player.dir = DOWN;
				break; 
			case SDLK_a: 
				player.dir = LEFT;
				break; 
			case SDLK_d: 
				player.dir = RIGHT;
				break;
			case SDLK_SPACE:
				player.shooting = 1;
				break; 
		}
	}
	else if(event.type == SDL_KEYUP) {
		switch(event.key.keysym.sym) { 

			case SDLK_UP: 
				player.yVel += PC_VEL; 
				break; 
			case SDLK_DOWN: 
				player.yVel -= PC_VEL; 
				break; 
			case SDLK_LEFT: 
				player.xVel += PC_VEL; 
				break; 
			case SDLK_RIGHT: 
				player.xVel -= PC_VEL; 
				break;
			case SDLK_SPACE:
				player.shooting = 0;
				break; 
		}
	}
}

void move_player() {

	//Move left or right 
	player.b.x += player.xVel; 
	//If went too far to the left or right 
	if((player.b.x < 0) || (player.b.x + PC_DIMS > SCREEN_WIDTH)) { 
		//move back 
		player.b.x -= player.xVel; 
	} 

	//Move up or down 
	player.b.y += player.yVel;

	//If went too far up or down 
	if((player.b.y < 0) || ( player.b.y + PC_DIMS > SCREEN_HEIGHT)) { 
		//move back 
		player.b.y -= player.yVel; 
	}
}

void move_enemies() {

	if(enemy.health < 0) return;

	int x1, y1, x2, y2;
	
	x1 = player.b.x;
	y1 = player.b.y;

	x2 = enemy.b.x; 
	y2 = enemy.b.y;

	//calculate x distance, move towards
	if((x1 - x2) < 0) {
		enemy.dir = LEFT;
		enemy.b.x -= EN_VEL;
	}
	else {
		enemy.dir = RIGHT;
		enemy.b.x += EN_VEL;
	}
	//calculate y distance, move towards
	if((y1 - y2) < 0){
		enemy.dir = UP;
		enemy.b.y -= EN_VEL;
	}
	else {
		enemy.dir = DOWN;
		enemy.b.y += EN_VEL;
	}

}

void draw_player() {

	switch(player.dir){
		case 0:
			if(player.frame > 7) player.frame = 4;
			apply_surface(player.b.x, player.b.y, playerSurf, screen, &player.animVer[player.frame++]);
			break;
		case 180:
			if(player.frame > 3) player.frame = 0;
			apply_surface(player.b.x, player.b.y, playerSurf, screen, &player.animVer[player.frame++]);
			break;
		case 270:
			if(player.frame > 3) player.frame = 0;
			apply_surface(player.b.x, player.b.y, playerSurf, screen, &player.animHor[player.frame++]);
			break;
		case 90:
			if(player.frame > 7) player.frame = 4;
			apply_surface(player.b.x, player.b.y, playerSurf, screen, &player.animHor[player.frame++]);
			break;
	}
}

void draw_enemies() {

	if(enemy.health < 0) {
		apply_surface(enemy.b.x, enemy.b.y, enemySurf, screen, &enemy.animVer[8]);
	}
	else {
		switch(enemy.dir){
			case 0:
				if(enemy.frame > 7) enemy.frame = 4;
				apply_surface(enemy.b.x, enemy.b.y, enemySurf, screen, &enemy.animVer[enemy.frame++]);
				break;
			case 180:
				if(enemy.frame > 3) enemy.frame = 0;
				apply_surface(enemy.b.x, enemy.b.y, enemySurf, screen, &enemy.animVer[enemy.frame++]);
				break;
			case 270:
				if(enemy.frame > 3) enemy.frame = 0;
				apply_surface(enemy.b.x, enemy.b.y, enemySurf, screen, &enemy.animHor[enemy.frame++]);
				break;
			case 90:
				if(enemy.frame > 7) enemy.frame = 4;
				apply_surface(enemy.b.x, enemy.b.y, enemySurf, screen, &enemy.animHor[enemy.frame++]);
				break;
		}
	}
}

void shoot_bullet() {

	switch(player.dir){
		case 0:
			lineRect.x = player.b.x + PC_DIMS*0.5;
			lineRect.y = player.b.y-300;
			lineRect.w = 1;
			lineRect.h = 300;
		break;

		case 180:
			lineRect.x = player.b.x + PC_DIMS*0.5;
			lineRect.y = player.b.y + PC_DIMS;
			lineRect.w = 1;
			lineRect.h = 300;
		break;

		case 270:
			lineRect.x = player.b.x-300;
			lineRect.y = player.b.y + PC_DIMS*0.5;
			lineRect.w = 300;
			lineRect.h = 1;
		break;

		case 90:
			lineRect.x = player.b.x + PC_DIMS;
			lineRect.y = player.b.y + PC_DIMS*0.5;
			lineRect.w = 300;
			lineRect.h = 1;
		break;
	}

	SDL_FillRect(screen, &lineRect, SDL_MapRGB(screen->format, 255, 255, 255));
}

int check_collision(SDL_Rect A, SDL_Rect B) {

    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = A.x;
    rightA = A.x + A.w;
    topA = A.y;
    bottomA = A.y + A.h;

    //Calculate the sides of rect B
    leftB = B.x;
    rightB = B.x + B.w;
    topB = B.y;
    bottomB = B.y + B.h;

    //If any of the sides from A are outside of B
    if(bottomA <= topB)
        return 0;

    if(topA >= bottomB)
        return 0;

    if(rightA <= leftB)
        return 0;

    if(leftA >= rightB)
        return 0;

    //If none of the sides from A are outside B
    return 1;
}
