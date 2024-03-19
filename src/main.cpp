#include <iostream>
#include <SDL2/SDL.h>
#include "display.h"

int main(int argc, char** argv){
    (void) argc;
    (void) argv;
    display display;
    if(!display.initSDL()) {exit(EXIT_FAILURE);}
    exit(EXIT_SUCCESS);
    return 0;
}