#include "display.h"

/**
 * default init for W/H, set to 64/32
*/
display::display(){
    _width = 64*20;
    _height = 32*20;
    _scale = 20;
    _forecolor = 0xFFFFFFFF;
    _backcolor = 0xFFD1DCFF;
}

/**
 * user set display size init for Display type
*/
display::display(uint32_t w, uint32_t h, uint32_t s, uint32_t fc, uint32_t bc){
    _scale = s;
    _width = w*_scale;
    _height = h*_scale;
    _forecolor = fc;
    _backcolor = bc;
}

/**
 * Init SDL
 * @return bool passed to main function to see if SDL init properly.
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
 * @return bool passed to main function to see if display init properly.
*/
bool display::createWindow(){
    _window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, 0);
    if(_window == NULL){
        SDL_Log("SDL Window could not init, %s\n", SDL_GetError());
        return false;
    }
    else{
        return true;
    }
}

/**
 * Init SDL Renderer subsystem
 * @return bool passed to main function to see if renderer init properly.
*/
bool display::createRenderer(){
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
    if(_renderer == NULL){
        SDL_Log("SDL Renderer could not init, %s\n", SDL_GetError());
        return false;
    }
    else{
        return true;
    }
}

/**
 * Clear the display, set to background color.
 * Bit shift to get only the specific bits for respective colors, 0xFF000000 -> 0xFF 
 * through bit shifting by 24 and casting to uint8_t
*/
void display::displayClear(){
    uint8_t r = (uint8_t)(_backcolor >> 24);
    uint8_t g = (uint8_t)(_backcolor >> 16);
    uint8_t b = (uint8_t)(_backcolor >> 8);
    uint8_t a = (uint8_t)(_backcolor >> 0);
    SDL_SetRenderDrawColor(_renderer, r, g, b, a);
    // SDL_Log("%d %d %d %d", r, g, b, a);
    SDL_RenderClear(_renderer); // for some reason "clears" the display with the current set render draw color...
}

void display::displayUpdate() const{
    SDL_RenderPresent(_renderer);
}

/**
 * Draw onto SDL display using renderer/window
*/
void display::displayDraw(){
    
}

/**
 * Handles input for the SDL window.
 * @param state of the loop, will break out of loop on certain cases
*/
void display::handleInput(int& state){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:
                state = 0;
                return;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym){
                case SDLK_ESCAPE:
                    state = 0;
                    return;
                default:
                    break;
                }
            case SDL_KEYUP:
                break;
            default:
                state = 1;
                return;
        }   
    }
}

/**
 * Clean system and free memory usage.
*/
void display::cleanSDL(){
    SDL_DestroyWindow(_window);
    SDL_DestroyRenderer(_renderer);
    _renderer = NULL;
    _window = NULL;
    SDL_Quit();
}