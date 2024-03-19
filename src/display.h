#ifndef DISPLAY_H
#define DISPLAY_H
#include <SDL2/SDL.h>

class display{
    public:
        display();
        display(uint16_t w, uint16_t h);
        bool initSDL() const;
        bool createWindow();
        bool createRenderer();
        void displayDraw();
        void cleanSDL();
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        uint16_t width;
        uint16_t height;
};
#endif