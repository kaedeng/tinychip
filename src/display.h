#ifndef DISPLAY_H
#define DISPLAY_H
#include <SDL2/SDL.h>

class display{
    public:
        display();
        display(u_int16_t w, u_int16_t h);
        bool initSDL() const;
        bool createWindow();
        bool createRenderer();
        void displayDraw();
        void cleanSDL();
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        u_int16_t width;
        u_int16_t height;
};
#endif