#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include "NESEMU.h"

class SDLEMUAPP
{
public:
    bool m_isStepByStep = false;
    explicit SDLEMUAPP(const char* title, int width, int height);
    ~SDLEMUAPP();

    bool isRunning() const { return m_isRunning; }
    void input();

    void poweron();
    void reset();
    void update();
    void step();
    void refresh();

    void get_path(const char* filename, char* filepath);
    void load(const char* filename);

private:
    int m_width, m_height;
    NESEMU *m_emu = nullptr;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_texture;
    bool m_isRunning = false;
    
};