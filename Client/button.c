#include "SDL/SDL.h"
#include "button.h"

void set_buttons() {

 	join_button.image = load_image("join.gif");
	join_button.box.x = SCREEN_CX-125;
	join_button.box.y = 225;
	join_button.box.w = 250;
	join_button.box.h = 100;

 	create_button.image = load_image("create.gif");
	create_button.box.x = SCREEN_CX-125;
	create_button.box.y = 350;
	create_button.box.w = 250;
	create_button.box.h = 100;

 	quit_button.image = load_image("quit.gif");
	quit_button.box.x = SCREEN_CX-125;
	quit_button.box.y = 475;
	quit_button.box.w = 250;
	quit_button.box.h = 100;

}

int handle_buttons() {

    int x = 0, y = 0;
	int button_ = IN_MENU;

    if(event.type == SDL_MOUSEBUTTONDOWN) {

        if(event.button.button == SDL_BUTTON_LEFT) {

            x = event.button.x;
            y = event.button.y;

            if((x > join_button.box.x) && (x < join_button.box.x + join_button.box.w) 
				&& (y > join_button.box.y) && (y < join_button.box.y + join_button.box.h)) {

				button_ = IN_GAME;
				
            }

            if((x > create_button.box.x) && (x < create_button.box.x + create_button.box.w) 
				&& (y > create_button.box.y) && (y < create_button.box.y + create_button.box.h)) {

				button_ = QUIT;
				
            }

            if((x > quit_button.box.x) && (x < quit_button.box.x + quit_button.box.w) 
				&& (y > quit_button.box.y) && (y < quit_button.box.y + quit_button.box.h)) {

				button_ = QUIT;
				
            }

        }
    }

	return button_;
}

void show_buttons() {

    apply_surface(join_button.box.x, join_button.box.y, join_button.image, screen, NULL);
    apply_surface(create_button.box.x, create_button.box.y, create_button.image, screen, NULL);
    apply_surface(quit_button.box.x, quit_button.box.y, quit_button.image, screen, NULL);
}

void free_buttons(){
	SDL_FreeSurface(join_button.image);
	SDL_FreeSurface(create_button.image);
	SDL_FreeSurface(quit_button.image);
}
