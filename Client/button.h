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
