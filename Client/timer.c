/*
* CT30A5002 Games and Networking 2013-14
* Authors:
*   Arttu Hanska (0342181) -  arttu.hanska@lut.fi
*   Joonas Blomberg (0342152) - joonas.blomberg@lut.fi
*   Markku Painomaa (0358551) - markku.painomaa@lut.fi
*/

#include "SDL/SDL.h"
#include "timer.h"

void init_timer() {
	start_ticks = SDL_GetTicks();
}

/* delta time */
int delta_time() {

	if(!start_ticks)
		return SDL_GetTicks() - start_ticks;
	else
		return 0;

}

