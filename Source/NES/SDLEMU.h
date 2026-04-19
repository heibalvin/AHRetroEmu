#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>

class SDLEMU
{
public:
    SDLEMU();
    ~SDLEMU();

    bool initialise(const char* title, int width, int height);
    void release();

    SDL_Event input();
    Uint8* load(const char* filename);
    void update();
    void refresh();

private:
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_texture = nullptr;
    Uint32* m_buffer = nullptr;

    int m_screenWidth = 0;
    int m_screenHeight = 0;
};