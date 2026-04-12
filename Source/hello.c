#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main(int argc, char *argv[]) {
    // 1. Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // 2. Create Window
    SDL_Window *window = SDL_CreateWindow("SDL3 No Callback", 640, 480, 0);
    if (!window) {
        SDL_Log("CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // 3. Main Loop
    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }
        // Drawing code would go here
    }

    // 4. Cleanup
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
