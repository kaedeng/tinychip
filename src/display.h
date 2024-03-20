#ifndef DISPLAY_H
#define DISPLAY_H
#include <SDL2/SDL.h>

class display{
    public:
        display();
        display(uint32_t w, uint32_t h, uint32_t s, uint32_t fc, uint32_t bc);
        bool initSDL() const;
        bool createWindow();
        bool createRenderer();
        void displayClear();
        void displayUpdate() const;
        void displayDraw();
        void handleInput(int& state);
        void cleanSDL();
    private:
        SDL_Window* _window;
        SDL_Renderer* _renderer;
        SDL_Texture* _texture;
        uint32_t _width;
        uint32_t _height;
        uint32_t _scale;
        uint32_t _forecolor;
        uint32_t _backcolor;
};
#endif