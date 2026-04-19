#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include "NESCMP.h"

class NESPPU: NESCMP {
public:
	NESPPU(NESEMU *emu);
	~NESPPU();

	void poweron();
	void reset();
	void update();

private:
	friend class NESBUS;
	Uint8 *memory; // Pointer to NES PPU registesr memory
};