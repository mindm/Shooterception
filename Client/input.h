/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#define NAME_LEN 24
#define MAX_LEN 100
#define LOGSIZE 5

/* Fonts and colors */
TTF_Font *title_font = NULL;
TTF_Font *text_font = NULL;

SDL_Color textColor = { 255, 33, 0 };
SDL_Color textShColor = { 0, 0, 0 };
SDL_Color chatColor = { 0xFF, 0xFF, 0xFF };

/* Text input */
int textinput = 0;
int textpos = 0;

char textbuffer[MAX_LEN] = { 0 };
char name_string[NAME_LEN] = { 0 };

/* Chatlog */
char chatlog[LOGSIZE][MAX_LEN];
int logposition = 0;
int logentries = 0;

/* Functions */

void setupStringInput(void);
void handleStringInput(int);
void addLog(char*, int);
void printText(int, int, char*, SDL_Color);
void printTitle(int, int, char*);
void printStringInput(int, int, char*, SDL_Color);
void resetStringInput(void);

char* itoa(int, char[]);

void freeFonts(void);

