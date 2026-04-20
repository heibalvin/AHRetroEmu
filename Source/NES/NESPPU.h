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

	Uint8 read(Uint16 addr);
	void write(Uint16 addr, Uint8 data);

	Uint16 readLE(Uint16 addr);
	void writeLE(Uint16 addr, Uint16 data);

private:
	friend class NESBUS;
	Uint8* m_registers; // Pointer to NES PPU registesr memory
	Uint8* m_vram; // Pointer to NES PPU VRAM memory
	Uint8* m_oam; // Pointer to NES PPU OAM memory
	Uint8* m_palette; // Pointer to NES PPU palette memory
};