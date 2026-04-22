#include <SDL3/SDL.h>
#include "SDLEMUAPP.h"
#include <stdlib.h>

SDLEMUAPP *g_app = nullptr;

int main(int, char*[])
{
    g_app = new SDLEMUAPP("AHNESEMU", 256, 240);
    if (!g_app->isRunning()) {
        return EXIT_FAILURE;
    }

    g_app->load("1942 (Japan, USA).nes");
    // g_app->load("cpu_dummy_reads.nes");
    g_app->poweron();

    // Main loop
    while (g_app->isRunning()) {
        g_app->input();
        if (g_app->m_isStepByStep == false) {
            g_app->update();
        }
    }

    // Shutdown emulator and SDL
    delete g_app;
    return EXIT_SUCCESS;
}