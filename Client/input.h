/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#define NAME_LEN 24
#define CHAT_LEN 100

/* Declarations */
TTF_Font *title_font = NULL;
TTF_Font *text_font = NULL;

SDL_Color textColor = { 255, 33, 0 };
SDL_Color textShColor = { 0, 0, 0 };
SDL_Color chatColor = { 0xFF, 0xFF, 0xFF };

char textbuffer[NAME_LEN] = { 0 };
char* name_string;
char* chat_string;

int textinput = 0;
int textpos = 0;

/* Functions */

void setup_string_input(void);
void handle_string_input(void);

void print_text(int, int, char*, SDL_Color);
void print_title(int, int, char*);
void print_string_input(int, int, SDL_Color);

char* itoa(int, char[]);

void free_fonts(void);

