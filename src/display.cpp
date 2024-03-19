#include "display.h"

/**
 * default init for W/H, set to 64/32
*/
display::display(){
    width = 64;
    height = 32;
}

/**
 * user set display size init for Display type
*/
display::display(uint16_t w, uint16_t h){
    width = w;
    height = h;
}

/**
 * Init SDL, return true if success, return false if fail
*/
bool display::initSDL() const{
    if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){
        SDL_Log("Could not init SDL systems, %s\n", SDL_GetError());
        return false;
    }
    else{
        return true;
    }
}

/**
 * Init SDL Window subsystem
*/
bool display::createWindow(){
    window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if(window == NULL){
        SDL_Log("SDL Window could not init, %s\n", SDL_GetError());
        return false;
    }
    else{
        return true;
    }
}

/**
 * Init SDL Renderer subsystem
*/
bool display::createRenderer(){
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL){
        SDL_Log("SDL Renderer could not init, %s\n", SDL_GetError());
        return false;
    }
    else{
        return true;
    }
}

/**
 * Draw onto SDL display using renderer/window
*/
void display::displayDraw(){

}

/**
 * Clean system and free mem usage.
*/
void display::cleanSDL(){
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);    
    SDL_Quit();
}