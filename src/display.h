#ifndef DISPLAY_H
#define DISPLAY_H
#include <SDL2/SDL.h>

class display{
    public:
        bool initSDL();
        bool createWindow();
        bool createRenderer();
        void displayDraw();
        void cleanSDL();
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
};
#endif