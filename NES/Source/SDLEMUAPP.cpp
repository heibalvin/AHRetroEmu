#include "SDLEMUAPP.h"

SDLEMUAPP::SDLEMUAPP(const char* title, int width, int height): m_width(width), m_height(height), m_emu(nullptr), m_window(nullptr), m_renderer(nullptr), m_texture(nullptr), m_isRunning(false) {
	// 1. Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDLEMUAPP: failed to initialize SDL: %s", SDL_GetError());
        return;
    }

    // 2. Create Window
    m_window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!m_window) {
        SDL_Log("SDLEMUAPP: failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return;
    }

    // Create renderer
    m_renderer = SDL_CreateRenderer(m_window,nullptr);
    if (!m_renderer) {
        SDL_Log("SDLEMUAPP: failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return;
    }

    // Set logical presentation with letterbox scaling
    if (!SDL_SetRenderLogicalPresentation(m_renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
        SDL_Log("SDLEMUAPP: failed to get render logical presentation: %s", SDL_GetError());
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return;
    }

    // Create screen texture for emulator framebuffer
    m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!m_texture) {
        SDL_Log("SDLEMUAPP: Failed to create screen texture: %s", SDL_GetError());
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return;
    }

    m_emu = new NESEMU();
    if (!m_emu) {
        SDL_Log("SDLEMUAPP: failed to create NESEMU instance");
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return;
    }

    m_isStepByStep = false;
    m_isRunning = true;
    SDL_Log("SDLEMU: SDL initialized successfully");
}

SDLEMUAPP::~SDLEMUAPP()
{
    m_isRunning = false;
    if (m_emu) {
        delete m_emu;
        m_emu = nullptr;
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
}

void SDLEMUAPP::input() {
	// Handle input events
	SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                m_isRunning = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.key) {
                    case SDLK_SPACE:
                        update();
                        m_isStepByStep = false;
                        break;
                    case SDLK_UP:
                        SDL_Log("SDLEMUAPP: KeyBoard Up Pressed");
                        break;
                    case SDLK_DOWN:
                        SDL_Log("SDLEMUAPP: KeyBoard Down Pressed");
                        break;
                    case SDLK_LEFT:
                        SDL_Log("SDLEMUAPP: KeyBoard Left Pressed");
                        break;
                    case SDLK_RIGHT:
                        SDL_Log("SDLEMUAPP: KeyBoard Right Pressed");
                        break;
                    case SDLK_W:
                        SDL_Log("SDLEMUAPP: KeyBoard W Pressed");
                        break;
                    case SDLK_X:
                        SDL_Log("SDLEMUAPP: KeyBoard X Pressed");
                        break;
                    case SDLK_ESCAPE:
                        m_isRunning = false;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

void SDLEMUAPP::get_path(const char* filename, char* filepath) {
    const char* basepath = SDL_GetBasePath();
    if (!basepath) {
        SDL_Log("SDLEMUAPP: failed to get base path: %s", SDL_GetError());
        return;
    }

    SDL_strlcpy(filepath, basepath, sizeof(filepath));
    SDL_strlcat(filepath, "../../Resources/ROMs/NES/", sizeof(filepath));
    SDL_strlcat(filepath, filename, sizeof(filepath));
    SDL_free((char *)basepath);
}

void SDLEMUAPP::load(const char* filename) {
    char filepath[512];

    const char* basepath = SDL_GetBasePath();
    if (!basepath) {
        SDL_Log("SDLEMUAPP: failed to get base path: %s", SDL_GetError());
        return;
    }

    SDL_strlcpy(filepath, basepath, sizeof(filepath));
    SDL_strlcat(filepath, "../../Resources/ROMs/NES/", sizeof(filepath));
    SDL_strlcat(filepath, filename, sizeof(filepath));
    SDL_free((char *)basepath);

    size_t filesize;
    Uint8* datas = (Uint8*)SDL_LoadFile(filepath, &filesize);
    if (!datas) {
        SDL_Log("SDLEMUAPP: failed to load file: %s", SDL_GetError());
        return;
    }
    m_emu->loadRom(datas);
    SDL_free(datas);
}

void SDLEMUAPP::poweron() {
    m_emu->poweron();
}

void SDLEMUAPP::update() {
    m_emu->update();
    if (m_emu->m_isRefreshReq) {
        refresh();
        m_emu->m_isRefreshReq = false;
        m_isStepByStep = true;
    }
}

void SDLEMUAPP::refresh() {
    void* pixels;
    int pitch;
    if (SDL_LockTexture(m_texture, nullptr, &pixels, &pitch)) {
        SDL_memcpy(pixels, m_emu->m_ppu->m_buffer, m_width * m_height * sizeof(Uint32));
        SDL_UnlockTexture(m_texture);
    }

    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);
    SDL_RenderTexture(m_renderer, m_texture, nullptr, nullptr);
    SDL_RenderPresent(m_renderer);

    SDL_Log("SDLEMUAPP: Refresh Required");
}
