#include <SDL3/SDL.h>
#include "SDLEMUAPP.h"
#include <stdlib.h>

SDLEMUAPP *g_app = nullptr;

int main(int, char*[])
{
    g_app = new SDLEMUAPP("AH NES Emulator", 256, 240);
    if (!g_app->isRunning()) {
        return EXIT_FAILURE;
    }

    g_app->load("1942 (Japan, USA).nes");
    g_app->poweron();

    // Main loop
    while (g_app->isRunning()) {
        g_app->input();

        // DEBUG: remove below for a step by step emulation using SPACEBAR
        /*
        g_app->update();
        */
    }

    // Shutdown emulator and SDL
    delete g_app;
    return EXIT_SUCCESS;
}

/*
#include <SDL3/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include "NESCMP.h"
#include "NESEMU.h"

// Screen constants
static const int SCREEN_WIDTH = 256;
static const int SCREEN_HEIGHT = 240;
bool isEmuUpdateRequired = false;
bool isRefreshRequired = false;

// SDL objects
static SDL_Window* g_window = nullptr;
static SDL_Renderer* g_renderer = nullptr;
static SDL_Texture* g_screen_texture = nullptr;
Uint32* g_screen_buffer = nullptr;

// NES Emulator
static std::unique_ptr<NESEMU> emu = nullptr;

bool initialize_sdl()
{
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    g_window = SDL_CreateWindow(
        "AH NES Emulator",
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );

    if (!g_window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    // Create renderer
    g_renderer = SDL_CreateRenderer(
        g_window,
        nullptr
    );

    if (!g_renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return false;
    }

    // Enable VSync
    SDL_SetRenderVSync(g_renderer, 1);

    // Set logical presentation for 256x240 with letterbox scaling
    SDL_SetRenderLogicalPresentation(g_renderer, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // Create screen texture for emulator framebuffer
    g_screen_texture = SDL_CreateTexture(
        g_renderer,
        SDL_PIXELFORMAT_XRGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    if (!g_screen_texture) {
        SDL_Log("Failed to create screen texture: %s", SDL_GetError());
        SDL_DestroyRenderer(g_renderer);
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return false;
    }

    // Allocate screen buffer
    g_screen_buffer = new Uint32[SCREEN_WIDTH * SCREEN_HEIGHT];
    if (!g_screen_buffer) {
        SDL_Log("Failed to allocate screen buffer");
        SDL_DestroyTexture(g_screen_texture);
        SDL_DestroyRenderer(g_renderer);
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return false;
    }

    // Initialize screen buffer with black
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
        g_screen_buffer[i] = 0xFF000000;
    }

    SDL_Log("SDL initialized successfully");
    return true;
}

void input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                break;
            case SDL_EVENT_KEY_DOWN:
                SDL_Log("Key pressed: %d", event.key.key);
                if (event.key.key == SDL_KEY_SPACE) {
                    isEmuUpdateRequired = true;
                }
                break;
            case SDL_EVENT_KEY_UP:
                SDL_Log("Key released: %d", event.key.key);
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                SDL_Log("Gamepad button pressed: %d", event.gbutton.button);
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                SDL_Log("Gamepad button released: %d", event.gbutton.button);
                break;
            default:
                break;
        }
    }
}

void refresh()
{
    if (!g_renderer || !g_screen_texture) return;

    void* pixels;
    int pitch;
    if (!SDL_LockTexture(g_screen_texture, nullptr, &pixels, &pitch)) {
        SDL_memcpy(pixels, g_screen_buffer, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));
        SDL_UnlockTexture(g_screen_texture);
    }

    SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
    SDL_RenderClear(g_renderer);
    SDL_RenderTexture(g_renderer, g_screen_texture, nullptr, nullptr);
    SDL_RenderPresent(g_renderer);
}

void cleanup_sdl()
{
    if (g_screen_buffer) {
        delete[] g_screen_buffer;
        g_screen_buffer = nullptr;
    }

    if (g_screen_texture) {
        SDL_DestroyTexture(g_screen_texture);
        g_screen_texture = nullptr;
    }

    if (g_renderer) {
        SDL_DestroyRenderer(g_renderer);
        g_renderer = nullptr;
    }

    if (g_window) {
        SDL_DestroyWindow(g_window);
        g_window = nullptr;
    }

    SDL_Quit();
    SDL_Log("SDL cleaned up");
}

int main(int, char*[])
{
    if (!initialize_sdl()) {
        return EXIT_FAILURE;
    }

    // Create and power on emulator
    emu = std::make_unique<NESEMU>();
    emu->poweron();

    // Main loop
    bool running = true;
    while (running) {
        input();

        if (isEmuUpdateRequired) {
            emu->update();
            isEmuUpdateRequired = false;
        }

        if (isRefreshRequired) {
                static int frame = 0;
            frame++;
            Uint32 color = 0xFFFF00FF | ((frame * 255 / 60) % 256) << 8;
            for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
                g_screen_buffer[i] = color;
            }

            refresh();
            isRefreshRequired = false;
        }
    }

    // Shutdown
    g_emulator = nullptr;
    cleanup_sdl();
    return EXIT_SUCCESS;
}
*/