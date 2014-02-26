/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "SDL/SDL.h"
#include "input.h"

void setupStringInput(){

	//texts and fonts
	title_font = TTF_OpenFont("OCR", 36);
	text_font = TTF_OpenFont("OCR", 16);
	resetStringInput();
}

void handleStringInput(int context, int length) {

    if(event.type == SDL_KEYDOWN) {
		
		textbuffer[textpos] = '|';
        if(textpos < length) {			
			//space
            if(event.key.keysym.unicode == (Uint16)' ') {
                textbuffer[textpos] = (char)event.key.keysym.unicode;
				textpos++; textbuffer[textpos] = '|';
			}

            //numeric
            else if((event.key.keysym.unicode >= (Uint16)'0') && (event.key.keysym.unicode <= (Uint16)'9')) {
                textbuffer[textpos] = (char)event.key.keysym.unicode;
				textpos++; textbuffer[textpos] = '|';
			}

            //uppercase letter
            else if((event.key.keysym.unicode >= (Uint16)'A') && (event.key.keysym.unicode <= (Uint16)'Z')) {
                textbuffer[textpos] = (char)event.key.keysym.unicode;
				textpos++; textbuffer[textpos] = '|';
			}

            //lowercase letter
            else if((event.key.keysym.unicode >= (Uint16)'a') && (event.key.keysym.unicode <= (Uint16)'z')) {
                textbuffer[textpos] = (char)event.key.keysym.unicode;
				textpos++; textbuffer[textpos] = '|';
			}

			

			//enter (if this is chat, then add log, otherwise ignore)
            else if(event.key.keysym.sym == SDLK_RETURN && length == MAX_LEN) {
				if(textpos > 0) {
					//memset(temp, 0, MAX_LEN);
					//textbuffer[textpos] = '\0';
					//sprintf(temp, "%s: %s", pl_name_str, textbuffer);
					sendChatMsg(textbuffer); 
					//addLog(temp, textpos+strlen(pl_name_str)+4);
				}
				resetStringInput();
			}	

        }

        //remove
        if((event.key.keysym.sym == SDLK_BACKSPACE) && (textpos > 0)) {							
				textpos--;
				textbuffer[textpos] = '|';
				textbuffer[textpos+1] = '\0';
				
		}

		//restrict length
		if(textpos == length-1) {
			textpos--;
		}

		if(context == NAME_MENU) {
			memcpy(&pl_name_str[0], textbuffer, textpos);
			pl_name_str[textpos+1] = '\0';
		}

		if(context == CREATE_MENU){
			memcpy(&g_name_str[0], textbuffer, textpos);
			g_name_str[textpos+1] = '\0';
	    }
    }
}

/* add log from ncurses example */
void addLog(char* message, int msglen) {
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
}

void resetStringInput(void) {
	textpos = 0;
	memset(&textbuffer, 0, MAX_LEN);
	textbuffer[textpos] = '|';
	textbuffer[textpos+1] = '\0';
}

//prints size 16 font
void printText(int x, int y, char* str, SDL_Color color) {

	SDL_Surface* temp = TTF_RenderText_Solid(text_font, str, color);
	
	applySurface(x, y, temp, screen, NULL);
	
	SDL_FreeSurface(temp);
}

//prints size 16 font
void printStringInput(int x, int y, char* string, SDL_Color color) {

	SDL_Surface* temp = TTF_RenderText_Solid(text_font, string, color);
	
	applySurface(x, y, temp, screen, NULL);
	
	SDL_FreeSurface(temp);
}

//prints size 36 font with cast shadow effect
void printTitle(int x, int y, char* str) {

	SDL_Surface* temp = TTF_RenderText_Solid(title_font, str, textShColor);
	SDL_Surface* temp2 = TTF_RenderText_Solid(title_font, str, textColor);

	applySurface(x-2, y-2, temp, screen, NULL);
	applySurface(x, y, temp2, screen, NULL);
	
	SDL_FreeSurface(temp); SDL_FreeSurface(temp2);
}

char* itoa(int i, char c[]) {
	
	char* s = c;
	sprintf(s, "%d", i);

	return s;
}

void freeFonts() {

	//free(name_string);

	TTF_CloseFont(title_font);
	TTF_CloseFont(text_font);
}
