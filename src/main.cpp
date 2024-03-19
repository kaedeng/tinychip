#include <iostream>
#include <SDL2/SDL.h>
#include "display.h"

int main(int argc, char** argv){
    (void) argc;
    (void) argv;

    display display;
    if(!display.initSDL()) {exit(EXIT_FAILURE);}
    if(!display.createWindow()) {exit(EXIT_FAILURE);}
    if(!display.createRenderer()) {exit(EXIT_FAILURE);}


    //temp loop
    while(true){
        SDL_Delay(16); // approx 60 hz delay
        display.displayClear();
        display.displayUpdate();
    }
    display.cleanSDL();
    exit(EXIT_SUCCESS);
    return 0;
}