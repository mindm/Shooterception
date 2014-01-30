/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

struct button {
	SDL_Rect box;
	SDL_Surface *image;
};

struct button join_button;
struct button create_button;
struct button quit_button;

int handle_buttons(void);
void show_buttons(void);
void free_buttons(void);
