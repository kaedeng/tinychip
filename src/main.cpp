#include <iostream>
#include <fstream>
#include <string>
#include <SDL2/SDL.h>
#include <time.h>
#include <stdlib.h>
#include "display.h"
#include "cpu.h"

int main(int argc, char** argv){
    srand((unsigned int)time(NULL));
    int state = 1; // 0 for quit, 1 for running, 2 for paused.

    display display;
    if(!display.initSDL()) {exit(EXIT_FAILURE);}
    if(!display.createWindow()) {exit(EXIT_FAILURE);}
    if(!display.createRenderer()) {exit(EXIT_FAILURE);}
    cpu cpu;
    char* rom;
    if(argc > 1) {
        rom = argv[1];
        if(!cpu.initCpu(rom, state)) {exit(EXIT_FAILURE);} 
    }
    // else {
    //     std::cout << "Input name of Rom: ";
    //     std::cin >> rom;
    //     std::cout << '\n';
    //     if(!cpu.initCpu(rom, state)) {exit(EXIT_FAILURE);} 
    // }
    display.displayClear();
    //temp loop
    while(state != 0){
        display.handleInput(state, &cpu);
        if(state == 2) {continue;}

        uint64_t countbefore = SDL_GetPerformanceCounter();
        for(uint32_t i = 0; i < 500/60; i++){
            cpu.emulateInst(display);
        }
        uint64_t countafter = SDL_GetPerformanceCounter();

        double timePass = (double)(((countafter-countbefore)/1000) / SDL_GetPerformanceFrequency());
        double targetFrameTime = 16.67;
        SDL_Delay(targetFrameTime > timePass ? targetFrameTime - timePass : 0);
        if(display.draw){
            display.displayDraw(&cpu);
            display.draw = false;
        }
        cpu.updateTimers();
    }
    display.cleanSDL();
    exit(EXIT_SUCCESS);
    return 0;
}