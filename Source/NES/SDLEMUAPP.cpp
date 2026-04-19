#include "SDLEMUAPP.h"

SDLEMUAPP::SDLEMUAPP(const char* title, int width, int height)
	: m_window(nullptr), m_renderer(nullptr)
{
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

    // Set logical presentation for 256x240 with letterbox scaling
    if (!SDL_SetRenderLogicalPresentation(m_renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
        SDL_Log("SDLEMUAPP: failed to get render logical presentation: %s", SDL_GetError());
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

    m_emu = new NESEMU();
    if (!m_emu) {
        SDL_Log("SDLEMUAPP: failed to create NESEMU instance");
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return;
    }

    m_isRunning = true;
    SDL_Log("SDLEMU: SDL initialized successfully");
}

SDLEMUAPP::~SDLEMUAPP()
{
    delete m_emu;
    m_emu = nullptr;

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
    // Implementation for loading a file
    const char* basepath = SDL_GetBasePath();
    if (!basepath) {
        SDL_Log("SDLEMUAPP: failed to get base path: %s", SDL_GetError());
        return;
    }

    SDL_strlcpy(filepath, basepath, sizeof(filepath));
    SDL_strlcat(filepath, "../../Resources/ROMs/NES/", sizeof(filepath));
    SDL_strlcat(filepath, filename, sizeof(filepath));
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

void SDLEMUAPP::reset() {
    m_emu->reset();
}

void SDLEMUAPP::update() {
    m_emu->update();
}