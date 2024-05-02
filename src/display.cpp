#include "display.h"
#include <iostream>
/**
 * default init for W/H, set to 64/32, scale to 20, forecolor to white, backcolor to black
*/
display::display(){
    _width = 64*20;
    _height = 32*20;
    _scale = 20;
    _forecolor = 0xFFFFFFFF; // 8 bit hex color for white ... 0xRRGGBBAA
    _backcolor = 0x00000000; // 8 bit hex color for black
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
 * through bit shifting by 24 and casting to uint8_t to get the lowest value of the 32 bit int.
*/
void display::displayClear(){
    uint8_t r = (uint8_t)(_backcolor >> 24);
    uint8_t g = (uint8_t)(_backcolor >> 16);
    uint8_t b = (uint8_t)(_backcolor >> 8);
    uint8_t a = (uint8_t)(_backcolor >> 0);
    SDL_SetRenderDrawColor(_renderer, r, g, b, a);
    SDL_RenderClear(_renderer);
}

/**
 * Function call to render into renderer/window
 * 
 */
void display::displayUpdate() const{
    SDL_RenderPresent(_renderer);
}
/**
 * Draw onto SDL display using renderer/window
*/
void display::displayDraw(cpu* cpu){
    SDL_Rect rect = {0, 0, (int)_scale, (int)_scale};
    const uint8_t bgR = (uint8_t)((_backcolor >> 24) & 0xFF);
    const uint8_t bgG = (uint8_t)((_backcolor >> 16) & 0xFF);
    const uint8_t bgB = (uint8_t)((_backcolor >>  8) & 0xFF);
    const uint8_t bgA = (uint8_t)((_backcolor >>  0) & 0xFF);

    const uint8_t fgR = (uint8_t)((_forecolor >> 24) & 0xFF);
    const uint8_t fgG = (uint8_t)((_forecolor >> 16) & 0xFF);
    const uint8_t fgB = (uint8_t)((_forecolor >>  8) & 0xFF);
    const uint8_t fgA = (uint8_t)((_forecolor >>  0) & 0xFF);

    for (uint32_t i = 0; i < sizeof(cpu->_display); i++) {
        // X = i % window width
        // Y = i / window width
        rect.x = (i % 64) * _scale;
        rect.y = (i / 64) * _scale;
        if(cpu->_display[i]){
            SDL_SetRenderDrawColor(_renderer, fgR, fgG, fgB, fgA);
            SDL_RenderFillRect(_renderer, &rect);
        }
        else{
            SDL_SetRenderDrawColor(_renderer, bgR, bgG, bgB, bgA);
            SDL_RenderFillRect(_renderer, &rect);            
        }   
    }
    SDL_RenderPresent(_renderer);
    //displayUpdate();
}
/**
 * Handles input for the SDL window.
 * @param state of the loop, will break out of loop on certain cases
*/
void display::handleInput(int& state, cpu* cpu){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:
                state = 0;
                return;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym){
                    case SDLK_ESCAPE: // Escape
                        state = 0;
                        return;
                    case SDLK_SPACE: // Space Bar
                        if(state == 1){state = 2; SDL_Log("--Paused--");}
                        else {state = 1; SDL_Log("--Unpaused--");}
                        return;
                    case SDLK_EQUALS:
                        // '=': Reset CHIP8 machine for the current ROM
                        cpu->initCpu(cpu->getName(), state);
                        break;
                    case SDLK_1: cpu->keymap[0x1] = true; break;
                    case SDLK_2: cpu->keymap[0x2] = true; break;
                    case SDLK_3: cpu->keymap[0x3] = true; break;
                    case SDLK_4: cpu->keymap[0xC] = true; break;

                    case SDLK_q: cpu->keymap[0x4] = true; break;
                    case SDLK_w: cpu->keymap[0x5] = true; break;
                    case SDLK_e: cpu->keymap[0x6] = true; break;
                    case SDLK_r: cpu->keymap[0xD] = true; break;

                    case SDLK_a: cpu->keymap[0x7] = true; break;
                    case SDLK_s: cpu->keymap[0x8] = true; break;
                    case SDLK_d: cpu->keymap[0x9] = true; break;
                    case SDLK_f: cpu->keymap[0xE] = true; break;

                    case SDLK_z: cpu->keymap[0xA] = true; break;
                    case SDLK_x: cpu->keymap[0x0] = true; break;
                    case SDLK_c: cpu->keymap[0xB] = true; break;
                    case SDLK_v: cpu->keymap[0xF] = true; break;

                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    // Map qwerty keys to CHIP8 keypad
                    case SDLK_1: cpu->keymap[0x1] = false; break;
                    case SDLK_2: cpu->keymap[0x2] = false; break;
                    case SDLK_3: cpu->keymap[0x3] = false; break;
                    case SDLK_4: cpu->keymap[0xC] = false; break;

                    case SDLK_q: cpu->keymap[0x4] = false; break;
                    case SDLK_w: cpu->keymap[0x5] = false; break;
                    case SDLK_e: cpu->keymap[0x6] = false; break;
                    case SDLK_r: cpu->keymap[0xD] = false; break;

                    case SDLK_a: cpu->keymap[0x7] = false; break;
                    case SDLK_s: cpu->keymap[0x8] = false; break;
                    case SDLK_d: cpu->keymap[0x9] = false; break;
                    case SDLK_f: cpu->keymap[0xE] = false; break;

                    case SDLK_z: cpu->keymap[0xA] = false; break;
                    case SDLK_x: cpu->keymap[0x0] = false; break;
                    case SDLK_c: cpu->keymap[0xB] = false; break;
                    case SDLK_v: cpu->keymap[0xF] = false; break;

                    default: break;
                }
                
            default:
                break;
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