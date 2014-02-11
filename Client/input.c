/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "SDL/SDL.h"
#include "input.h"

void setup_string_input(){

	name_string = malloc(sizeof(char)*NAME_LEN);
	name_string = textbuffer;

	//texts and fonts
	title_font = TTF_OpenFont("OCR", 36);
	text_font = TTF_OpenFont("OCR", 16);
}

void handle_string_input() {

    if(event.type == SDL_KEYDOWN) {

        if(textpos < NAME_LEN) {

			//space
            if(event.key.keysym.unicode == (Uint16)' ' ){
                textbuffer[textpos] = (char)event.key.keysym.unicode;
				textpos++;
			}

            //numeric
            else if((event.key.keysym.unicode >= (Uint16)'0') && (event.key.keysym.unicode <= (Uint16)'9')) {
                textbuffer[textpos] = (char)event.key.keysym.unicode;
				textpos++;
			}

            //uppercase letter
            else if((event.key.keysym.unicode >= (Uint16)'A') && (event.key.keysym.unicode <= (Uint16)'Z')) {
                textbuffer[textpos] = (char)event.key.keysym.unicode;
				textpos++;
			}

            //lowercase letter
            else if((event.key.keysym.unicode >= (Uint16)'a') && (event.key.keysym.unicode <= (Uint16)'z')) {
                textbuffer[textpos] = (char)event.key.keysym.unicode;
				textpos++;
			}
        }

        //remove
        if((event.key.keysym.sym == SDLK_BACKSPACE) && (textpos > 0)) {
				textpos--;
				textbuffer[textpos] = '\0';
		}

		if(textpos == NAME_LEN-1) {
			textpos = 0;
			textinput = 0;
		}

		name_string = textbuffer;
    }
}

/* add log from ncurses example */
/*void add_log(char* message, int msglen) {
	// Log is less than maximum
	if(logposition < LOGSIZE) {
		memset(&chatlog[logposition][0], 0, MAX_LEN);
		memcpy(&chatlog[logposition][0], message, msglen); // Copy to the position
		logposition++; // Increase counter
	}
	// Log is full
	else {
		// Move the entries 
		for(int i = 0; i < LOGSIZE-1; i++) 
			memcpy(&chatlog[i][0], &chatlog[i+1][0], MAX_LEN);

		memset(&chatlog[LOGSIZE-1][0], 0, MAX_LEN);
		memcpy(&chatlog[LOGSIZE-1][0], message, msglen); // Replace the last with the new one
		logentries++; // Increase counter
	}
}*/

//prints size 16 font
void print_text(int x, int y, char* str, SDL_Color color) {

	SDL_Surface* temp = TTF_RenderText_Solid(text_font, str, color);
	
	apply_surface(x, y, temp, screen, NULL);
	
	SDL_FreeSurface(temp);
}

//prints size 16 font
void print_string_input(int x, int y, SDL_Color color) {


	SDL_Surface* temp = TTF_RenderText_Solid(text_font, name_string, color);
	
	apply_surface(x, y, temp, screen, NULL);
	
	SDL_FreeSurface(temp);
}

//prints size 36 font with cast shadow effect
void print_title(int x, int y, char* str) {

	SDL_Surface* temp = TTF_RenderText_Solid(title_font, str, textShColor);
	SDL_Surface* temp2 = TTF_RenderText_Solid(title_font, str, textColor);

	apply_surface(x-2, y-2, temp, screen, NULL);
	apply_surface(x, y, temp2, screen, NULL);
	
	SDL_FreeSurface(temp); SDL_FreeSurface(temp2);
}

char* itoa(int i, char c[]) {
	
	char* s = c;
	sprintf(s, "%d", i);

	return s;
}

void free_fonts() {


	//free(name_string);

	TTF_CloseFont(title_font);
	TTF_CloseFont(text_font);
}
