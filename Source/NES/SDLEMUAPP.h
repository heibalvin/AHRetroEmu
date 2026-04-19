#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include "NESEMU.h"

class SDLEMUAPP
{
public:
    explicit SDLEMUAPP(const char* title, int width, int height);
    ~SDLEMUAPP();

    bool isRunning() const { return m_isRunning; }
    void input();

    void poweron();
    void reset();
    void update();

    void get_path(const char* filename, char* filepath);
    void load(const char* filename);

private:
    NESEMU *m_emu = nullptr;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    bool m_isRunning = false;
};