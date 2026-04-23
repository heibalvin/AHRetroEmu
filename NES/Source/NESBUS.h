
#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include "NESCMP.h"

class NESBUS: NESCMP {
public:
	NESBUS(NESEMU *emu);
	~NESBUS();

	Uint8 read(Uint16 addr);
	void write(Uint16 addr, Uint8 data);

	Uint16 readWord(Uint16 addr);
	void writeWord(Uint16 addr, Uint16 data);

	Uint16 readBE(Uint16 addr);
	void writeBE(Uint16 addr, Uint16 data);

private:
	friend class NESCPU;
	friend class NESPPU;
	Uint8* m_apu_registers;
	Uint8* m_io_registers;
};