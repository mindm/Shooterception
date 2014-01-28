#include "SDL/SDL.h"
#include "timer.h"

void startTimer() {
    //Start the timer
    started = 1;

    //Unpause the timer
    paused = 0;

    //Get the current clock time
    startTicks = SDL_GetTicks();
}

void stopTimer() {
    //Stop the timer
    started = 0;

    //Unpause the timer
    paused = 0;
}

int get_ticks() {
    //If the timer is running
    if(started)
    {
        //If the timer is paused
        if(paused)
        {
            //Return the number of ticks when the timer was paused
            return pausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            return SDL_GetTicks() - startTicks;
        }
    }

    //If the timer isn't running
    return 0;
}

void pauseTimer(){
    //If the timer is running and isn't already paused
    if(started && !paused)
    {
        //Pause the timer
        paused = 1;

        //Calculate the paused ticks
        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void unpauseTimer(){
    //If the timer is paused
    if(paused)
    {
        //Unpause the timer
        paused = 0;

        //Reset the starting ticks
        startTicks = SDL_GetTicks() - pausedTicks;

        //Reset the paused ticks
        pausedTicks = 0;
    }
}
