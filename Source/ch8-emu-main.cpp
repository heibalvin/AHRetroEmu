#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "ch8-emu.h"
#include <cstdio>
#include <string>

#define SCALE 10
#define SCREEN_WIDTH (CH8Emu::DISPLAY_WIDTH * SCALE)
#define SCREEN_HEIGHT (CH8Emu::DISPLAY_HEIGHT * SCALE)

static const uint8_t keymap[CH8Emu::KEYS] = {
    SDLK_1, SDLK_2, SDLK_3, SDLK_4,
    SDLK_Q, SDLK_W, SDLK_E, SDLK_R,
    SDLK_A, SDLK_S, SDLK_D, SDLK_F,
    SDLK_Z, SDLK_X, SDLK_C, SDLK_V
};

static const char* test_roms[7] = { "1-chip8-logo.ch8", "2-ibm-logo.ch8",  "3-corax+.ch8", "4-flags.ch8", "5-quirks.ch8", "6-keypad.ch8", "7-beep.ch8" };
static const char* default_rom = test_roms[5];

static uint8_t sdlKeymap(SDL_Keycode key) {
    for (uint8_t i = 0; i < CH8Emu::KEYS; i++) {
        if (keymap[i] == key) return i;
    }
    return 0xFF;
}

static char resourcePath[512];

const char* getResourcePath(const char* filename) {
    const char* base_path = SDL_GetBasePath();
    
    if (base_path) {
        char* res = SDL_strstr(base_path, "Resources");
        if (res) {
            SDL_strlcpy(resourcePath, base_path, sizeof(resourcePath));
        } else {
            SDL_strlcpy(resourcePath, base_path, sizeof(resourcePath));
            SDL_strlcat(resourcePath, "../Resources/", sizeof(resourcePath));
        }
        SDL_strlcat(resourcePath, filename, sizeof(resourcePath));
        SDL_free((void*)base_path);
        return resourcePath;
    }
    return "";
}

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("CH8 Emulator", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window) {
        SDL_Log("CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    CH8Emu emu;
    emu.init();

    const char* romFile = default_rom;
    if (argc > 1 && argv[1] && SDL_strstr(argv[1], ".ch8")) {
        romFile = argv[1];
    }
    const char* romPath = getResourcePath(romFile);
    
    size_t dataSize = 0;
    void* romData = SDL_LoadFile(romPath, &dataSize);
    if (romData && dataSize > 0) {
        emu.loadROM(static_cast<uint8_t*>(romData), static_cast<uint32_t>(dataSize));
        SDL_free(romData);
    } else {
        SDL_Log("Error: Loading ROM from: %s", romPath);
    }

    Uint64 lastTick = SDL_GetTicks();
    Uint64 timerTick = SDL_GetTicks();

    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
                case SDL_EVENT_KEY_DOWN: {
                    uint8_t key = sdlKeymap(event.key.key);
                    if (key < CH8Emu::KEYS) {
                        emu.setKey(key, true);
                    }
                    if (event.key.key == SDLK_ESCAPE) {
                        quit = true;
                    }
                    break;
                }
                case SDL_EVENT_KEY_UP: {
                    uint8_t key = sdlKeymap(event.key.key);
                    if (key < CH8Emu::KEYS) {
                        emu.setKey(key, false);
                    }
                    break;
                }
            }
        }

        Uint64 now = SDL_GetTicks();
        if (now - lastTick >= 16) {
            emu.step();
            lastTick = now;
        }

        if (now - timerTick >= 16) {
            emu.timersTick();
            timerTick = now;
        }

        if (emu.drawFlag) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            for (int y = 0; y < CH8Emu::DISPLAY_HEIGHT; y++) {
                for (int x = 0; x < CH8Emu::DISPLAY_WIDTH; x++) {
                    if (emu.display[y][x]) {
                        SDL_FRect rect = {
                            (float)(x * SCALE),
                            (float)(y * SCALE),
                            (float)SCALE,
                            (float)SCALE
                        };
                        SDL_RenderFillRect(renderer, &rect);
                    }
                }
            }
            SDL_RenderPresent(renderer);
            emu.drawFlag = false;
        }

        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
