#include <iostream>
#include <fstream>
#include <string>
#include <SDL2/SDL.h>
#include "display.h"
#include "cpu.h"

int main(int argc, char** argv){

    int state = 1; // 0 for quit, 1 for running, 2 for paused.

    display display;
    if(!display.initSDL()) {exit(EXIT_FAILURE);}
    if(!display.createWindow()) {exit(EXIT_FAILURE);}
    if(!display.createRenderer()) {exit(EXIT_FAILURE);}
    cpu cpu;
    std::string rom;
    if(argc > 1) {
        rom = argv[1];
        if(!cpu.initCpu(rom, state)) {exit(EXIT_FAILURE);} 
    }
    else {
        std::cout << "Input name of Rom: ";
        std::cin >> rom;
        std::cout << '\n';
        if(!cpu.initCpu(rom, state)) {exit(EXIT_FAILURE);} 
    }


    //temp loop
    while(state != 0){
        display.handleInput(state);
        if(state == 2) {continue;}
        cpu.emulateInst(display);
        SDL_Delay(16); // approx 60 hz delay
        if(display.draw){
        display.displayDraw(&cpu);
            display.draw = false;
        }
        
    }
    display.cleanSDL();
    exit(EXIT_SUCCESS);
    return 0;
}