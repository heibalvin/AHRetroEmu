#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "ch8-emu.h"
#include <cstdio>
#include <string>

#define SCALE 10
#define SCREEN_WIDTH (CH8Emu::DISPLAY_WIDTH * SCALE)
#define SCREEN_HEIGHT (CH8Emu::DISPLAY_HEIGHT * SCALE)

static const SDL_Scancode keymap[CH8Emu::KEYS] = {
    SDL_SCANCODE_X,          // 0 (row 4, col 2)
    SDL_SCANCODE_1,          // 1 (row 1, col 1)
    SDL_SCANCODE_2,          // 2 (row 1, col 2)
    SDL_SCANCODE_3,          // 3 (row 1, col 3)
    SDL_SCANCODE_Q,          // 4 (row 2, col 1)
    SDL_SCANCODE_W,          // 5 (row 2, col 2)
    SDL_SCANCODE_E,          // 6 (row 2, col 3)
    SDL_SCANCODE_A,          // 7 (row 3, col 1)
    SDL_SCANCODE_S,          // 8 (row 3, col 2)
    SDL_SCANCODE_D,          // 9 (row 3, col 3)
    SDL_SCANCODE_Z,          // A (row 4, col 1)
    SDL_SCANCODE_C,          // B (row 4, col 3)
    SDL_SCANCODE_4,          // C (row 1, col 4)
    SDL_SCANCODE_R,          // D (row 2, col 4)
    SDL_SCANCODE_F,          // E (row 3, col 4)
    SDL_SCANCODE_V           // F (row 4, col 4)
};

static const char* test_roms[7] = { "1-chip8-logo.ch8", "2-ibm-logo.ch8",  "3-corax+.ch8", "4-flags.ch8", "5-quirks.ch8", "6-keypad.ch8", "7-beep.ch8" };
static const char* default_rom = test_roms[5];

static uint8_t sdlKeymap(SDL_Scancode scancode) {
    for (uint8_t i = 0; i < CH8Emu::KEYS; i++) {
        if (keymap[i] == scancode) return i;
    }
    return 0xFF;
}

static char resourceBase[512];

void initResourceBase() {
    const char* base_path = SDL_GetBasePath();
    if (base_path) {
        char* res = SDL_strstr(base_path, "Resources");
        if (res) {
            SDL_strlcpy(resourceBase, base_path, sizeof(resourceBase));
        } else {
            SDL_strlcpy(resourceBase, base_path, sizeof(resourceBase));
            SDL_strlcat(resourceBase, "../Resources/", sizeof(resourceBase));
            SDL_strlcat(resourceBase, "/", sizeof(resourceBase));
        }
        SDL_free((void*)base_path);
    }
}

const char* getResourcePath(const char* filename) {
    static char resourcePath[512];
    SDL_strlcpy(resourcePath, resourceBase, sizeof(resourcePath));
    SDL_strlcat(resourcePath, filename, sizeof(resourcePath));
    return resourcePath;
}

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    initResourceBase();

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

    if (!(SDL_Init(SDL_INIT_AUDIO))) {
        SDL_Log("Audio init failed: %s", SDL_GetError());
    }

    SDL_AudioSpec beepSpec;
    Uint32 beepLength = 0;
    Uint8* beepBuffer = nullptr;
    const char* beepPath = getResourcePath("beep.wav");
    if (SDL_LoadWAV(beepPath, &beepSpec, &beepBuffer, &beepLength)) {
        SDL_Log("Loaded beep: %u bytes, freq=%d, format=%d, channels=%d", 
            beepLength, beepSpec.freq, beepSpec.format, beepSpec.channels);
    } else {
        SDL_Log("Failed to load beep: %s", beepPath);
    }

    SDL_AudioSpec buzzSpec;
    Uint32 buzzLength = 0;
    Uint8* buzzBuffer = nullptr;
    const char* buzzPath = getResourcePath("buzz.wav");
    if (SDL_LoadWAV(buzzPath, &buzzSpec, &buzzBuffer, &buzzLength)) {
        SDL_Log("Loaded buzz: %u bytes, freq=%d, format=%d, channels=%d", 
            buzzLength, buzzSpec.freq, buzzSpec.format, buzzSpec.channels);
    } else {
        SDL_Log("Failed to load buzz: %s", buzzPath);
    }

    SDL_AudioStream* audioStream = nullptr;
    if (beepBuffer || buzzBuffer) {
        audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &beepSpec, NULL, NULL);
        if (!audioStream) {
            SDL_Log("Failed to open audio stream: %s", SDL_GetError());
        } else {
            SDL_ResumeAudioStreamDevice(audioStream);
        }
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
        SDL_Log("Successfully loaded ROM: %s", romFile);
    } else {
        SDL_Log("Failed to load ROM from: %s", romPath);
    }

    Uint64 lastFrameTick = SDL_GetTicks();
    static constexpr int CPU_STEPS_PER_FRAME = 1;
    static constexpr int FRAME_MS = 17;

    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
                case SDL_EVENT_KEY_DOWN: {
                    uint8_t key = sdlKeymap(event.key.scancode);
                    if (key < CH8Emu::KEYS) {
                        emu.setKey(key, true);
                    }
                    if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                        quit = true;
                    }
                    break;
                }
                case SDL_EVENT_KEY_UP: {
                    uint8_t key = sdlKeymap(event.key.scancode);
                    if (key < CH8Emu::KEYS) {
                        emu.setKey(key, false);
                    }
                    break;
                }
            }
        }

        for (int i = 0; i < CPU_STEPS_PER_FRAME; i++) {
            emu.step();
        }

        Uint64 now = SDL_GetTicks();
        if (now - lastFrameTick >= FRAME_MS) {
            emu.timersTick();
            if (emu.beepFlag && audioStream && beepBuffer) {
                int shortBeep = 4410;
                SDL_PutAudioStreamData(audioStream, beepBuffer, shortBeep);
                emu.beepFlag = false;
            }
            if (emu.soundFlag && audioStream && buzzBuffer) {
                int shortBuzz = 4410;
                SDL_PutAudioStreamData(audioStream, buzzBuffer, shortBuzz);
                emu.soundFlag = false;
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

            lastFrameTick = now;
        }

        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    if (audioStream) {
        SDL_DestroyAudioStream(audioStream);
    }
    SDL_free(beepBuffer);
    SDL_free(buzzBuffer);
    SDL_Quit();
    return 0;
}
