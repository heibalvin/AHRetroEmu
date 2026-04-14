#include <SDL3/SDL.h>
#include <cstdio>
#include <cstdlib>

// Screen constants
static const int SCREEN_WIDTH = 256;
static const int SCREEN_HEIGHT = 240;
static const float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT;

// SDL objects
static SDL_Window* g_window = nullptr;
static SDL_Renderer* g_renderer = nullptr;
static SDL_Texture* g_screen_texture = nullptr;
static uint32_t* g_screen_buffer = nullptr;

bool initialize_sdl()
{
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Create window with aspect ratio preserved
    const int INITIAL_WINDOW_WIDTH = 512;
    const int INITIAL_WINDOW_HEIGHT = 480;

    g_window = SDL_CreateWindow(
        "AH NES Emulator",
        INITIAL_WINDOW_WIDTH,
        INITIAL_WINDOW_HEIGHT,
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
    g_screen_buffer = new uint32_t[SCREEN_WIDTH * SCREEN_HEIGHT];
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

void handle_keyboard_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                break;
            case SDL_EVENT_KEY_DOWN:
                SDL_Log("Key pressed: %d", event.key.key);
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

void update_screen()
{
    if (!g_renderer || !g_screen_texture) return;

    // Update texture with screen buffer
    void* pixels;
    int pitch;
    if (!SDL_LockTexture(g_screen_texture, nullptr, &pixels, &pitch)) {
        SDL_memcpy(pixels, g_screen_buffer, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
        SDL_UnlockTexture(g_screen_texture);
    }

    // Get window size
    int window_width, window_height;
    SDL_GetWindowSize(g_window, &window_width, &window_height);

    // Calculate aspect ratio preserving rect
    float window_aspect = static_cast<float>(window_width) / window_height;
    SDL_FRect dest_rect;

    if (window_aspect > ASPECT_RATIO) {
        // Window is wider than needed, letterbox sides
        float new_width = window_height * ASPECT_RATIO;
        dest_rect.w = new_width;
        dest_rect.h = window_height;
        dest_rect.x = (window_width - new_width) / 2.0f;
        dest_rect.y = 0;
    } else {
        // Window is taller than needed, pillarbox top/bottom
        float new_height = window_width / ASPECT_RATIO;
        dest_rect.w = window_width;
        dest_rect.h = new_height;
        dest_rect.x = 0;
        dest_rect.y = (window_height - new_height) / 2.0f;
    }

    // Clear and render
    SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
    SDL_RenderClear(g_renderer);
    SDL_RenderTexture(g_renderer, g_screen_texture, nullptr, &dest_rect);
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

    // Main loop
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        handle_keyboard_input();
        update_screen();

        // Simple test pattern - cycle colors
        static int frame = 0;
        frame++;
        uint32_t color = 0xFF000000 | ((frame * 255 / 60) % 256) << 8;
        for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
            g_screen_buffer[i] = color;
        }
    }

    cleanup_sdl();
    return EXIT_SUCCESS;
}
