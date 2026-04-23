
#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include "NESCMP.h"

class NESDSK: NESCMP {
public:
	NESDSK(NESEMU *emu);
	~NESDSK();

	void load(Uint8 *datas);
	char* toJSON();

private:
	friend class NESBUS;
	friend class NESPPU;
	int version;
	int mapper;
	int mirroring;
	int tv_system;

	int prg_rom_size;
	int prg_rom_count;
	Uint8 **prg_roms;

	int chr_rom_size;
	int chr_rom_count;
	Uint8 **chr_roms;

	int active_prg_rom[2] = { 0, 0 };
	int active_chr_rom[2] = { 0, 0 };
};