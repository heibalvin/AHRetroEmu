#include "SDLEMU.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

SDLEMU::SDLEMU()
{
}

SDLEMU::~SDLEMU()
{
    release();
}

bool SDLEMU::initialise(const char* title, int width, int height)
{
    m_screenWidth = width;
    m_screenHeight = height;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    m_window = SDL_CreateWindow(
        title,
        width,
        height,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );

    if (!m_window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    m_renderer = SDL_CreateRenderer(m_window, nullptr);
    if (!m_renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return false;
    }

    SDL_SetRenderVSync(m_renderer, 1);
    SDL_SetRenderLogicalPresentation(m_renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    m_texture = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_XRGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );

    if (!m_texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return false;
    }

    m_buffer = new Uint32[width * height];
    if (!m_buffer) {
        SDL_Log("Failed to allocate screen buffer");
        SDL_DestroyTexture(m_texture);
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return false;
    }

    for (int i = 0; i < width * height; ++i) {
        m_buffer[i] = 0xFF000000;
    }

    SDL_Log("SDLEMU initialized: %dx%d", width, height);
    return true;
}

void SDLEMU::release()
{
    if (m_buffer) {
        delete[] m_buffer;
        m_buffer = nullptr;
    }

    if (m_texture) {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }

    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }

    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }

    SDL_Quit();
    SDL_Log("SDLEMU released");
}

SDL_Event SDLEMU::input()
{
    SDL_Event event;
    SDL_PollEvent(&event);
    return event;
}

Uint8* SDLEMU::loadNesRom(const char* filename)
{
    return nullptr;
}

void SDLEMU::update()
{
}

void SDLEMU::refresh()
{
    if (!m_renderer || !m_texture || !m_buffer) return;

    void* pixels;
    int pitch;
    if (!SDL_LockTexture(m_texture, nullptr, &pixels, &pitch)) {
        SDL_memcpy(pixels, m_buffer, m_screenWidth * m_screenHeight * sizeof(Uint32));
        SDL_UnlockTexture(m_texture);
    }

    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);
    SDL_RenderTexture(m_renderer, m_texture, nullptr, nullptr);
    SDL_RenderPresent(m_renderer);
}