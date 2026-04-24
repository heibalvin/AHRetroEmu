#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include "NESCMP.h"

class NESPPU: NESCMP {
public:
	NESPPU(NESEMU *emu);
	~NESPPU();

	void toJSON(char *jsonstring);
	char *dump();

	void poweron();
	void update();

	Uint8 read(Uint16 addr);
	void write(Uint16 addr, Uint8 data);

	Uint32* buffer() { return m_buffer; }
	bool isVblank() const { return m_isVblank != 0; }
	int getScanline() const { return m_scanline; }

private:
	friend class SDLEMUAPP;
	friend class NESBUS;
	friend class NESCPU;
	friend class NESDSK;
	int m_width;
	int m_height;
	Uint8* m_registers;
	Uint8* m_vram;
	Uint8* m_oam;
	Uint8* m_palette;
	Uint32* m_buffer;
	int m_dot = 0;
	int m_scanline = 0;
	int m_isVblank = false;

	static const Uint32 m_palette_rgb[65];
};
