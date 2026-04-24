#include "NESPPU.h"
#include "NESEMU.h"
#include "NESCPU.h"

const Uint32 NESPPU::m_palette_rgb[65] = { 0x000000, 0xfcfcfc, 0xf8f8f8, 0xbcbcbc, 0x7c7c7c, 0xa4e4fc, 0x3cbcfc, 0x0078f8, 0x0000fc, 0xb8b8f8, 0x6888fc, 0x0058f8, 0x0000bc, 0xd8b8f8, 0x9878f8, 0x6844fc, 0x4428bc, 0xf8b8f8, 0xf878f8, 0xd800cc, 0x940084, 0xf8a4c0, 0xf85898, 0xe40058, 0xa80020, 0xf0d0b0, 0xf87858, 0xf83800, 0xa81000, 0xfce0a8, 0xfca044, 0xe45c10, 0x881400, 0xf8d878, 0xf8b800, 0xac7c00, 0x503000, 0xd8f878, 0xb8f818, 0x00b800, 0x007800, 0xb8f8b8, 0x58d854, 0x00a800, 0x006800, 0xb8f8d8, 0x58f898, 0x00a844, 0x005800, 0x00fcfc, 0x00e8d8, 0x008888, 0x004058, 0xf8d8f8, 0x787878 };

NESPPU::NESPPU(NESEMU *emu) : NESCMP(emu), m_width(341), m_height(262) {
	m_registers = (Uint8 *)SDL_malloc(8 * sizeof(Uint8));
	m_vram = (Uint8*)SDL_malloc(0x0800 * sizeof(Uint8));
	m_oam = (Uint8*)SDL_malloc(256 * sizeof(Uint8));
	m_palette = (Uint8*)SDL_malloc(32 * sizeof(Uint8));
	m_buffer = (Uint32*)SDL_malloc(m_width * m_height * sizeof(Uint32));
}

NESPPU::~NESPPU() {
	if (m_registers) {
		SDL_free(m_registers);
		m_registers = nullptr;
	}
	if (m_vram) {
		SDL_free(m_vram);
		m_vram = nullptr;
	}
	if (m_oam) {
		SDL_free(m_oam);
		m_oam = nullptr;
	}
	if (m_palette) {
		SDL_free(m_palette);
		m_palette = nullptr;
	}
	if (m_buffer) {
		SDL_free( m_buffer);
		m_buffer = nullptr;
	}
}

char* NESPPU::dump() {
	char *str = (char *)SDL_malloc(sizeof(char)*80);
	SDL_snprintf(str, 80,
		"D: %03d, L: %03d, PPUCTRL: %02X, PPUMASK: %02X, PPUSTATUS: %02X", m_dot, m_scanline, m_emu->m_bus->read(0x2000), m_emu->m_bus->read(0x2001), m_emu->m_bus->read(0x2002));
	return str;
}

void NESPPU::poweron() {
	for (int i = 0; i < 8; ++i) {
		m_registers[i] = 0x00;
	}
	m_dot = 0;
	m_scanline = 0;
	for (int i = 0; i < m_width * m_height; ++i) {
		m_buffer[i] = 0xFF000000;
	}
	
	char *str = dump();
	SDL_Log("NESPPU: poweron: %s", str);
	SDL_free(str);
}

void NESPPU::update() {
	Uint32 color = 0xFF000000;
	if ((m_dot < m_width) && (m_scanline < m_height)) {
		color = ((m_scanline << 8) & 0xFF00)| (m_dot & 0x00FF);
		m_buffer[m_scanline * m_width + m_dot] = color;
	}
	
	m_dot++;
	if (m_dot >= m_width) {
		// Line Event
		m_dot = 0;
		m_scanline++;

		// trigger EMU event
		m_emu->m_isLineEvent = true;
	}

	if (m_scanline == 240 && m_dot == 0) {
		// VBlank Event
		m_registers[2] = m_registers[2] | 0x80;
		
		// set internal flag
		m_isVblank = true;
		
		// trigger CPU NMI
		m_emu->m_cpu->m_isNMIInterruptReq = true;
		
		// trigger EMU event for App SDL3 refresh
		m_emu->m_isVBlankEvent = true;
	}

	if (m_scanline >= 262) {
		// FRAME
		m_scanline = 0;
		m_dot = 0;
		m_registers[2] = m_registers[2] & ~0x80;

		// unset internal flag
		m_isVblank = false;

		// trigger EMU event
		m_emu->m_isFrameEvent = true;
	}

	char *str = dump();
	SDL_Log("NESPPU: update: %s", str);
	SDL_free(str);
}


Uint8 NESPPU::read(Uint16 addr) {
	if (addr >= 0x0000 && addr < 0x1000) {
		return m_emu->m_dsk->chr_roms[m_emu->m_dsk->active_chr_rom[0]][addr % 0x1000];
	} else if (addr >= 0x1000 && addr < 0x2000) {
		return m_emu->m_dsk->chr_roms[m_emu->m_dsk->active_chr_rom[1]][addr % 0x1000];
	} else if (addr >= 0x2000 && addr < 0x3000) {
		return m_vram[(addr - 0x2000) % 0x0800];
	} else if (addr >= 0x3000 && addr < 0x3F00) {
		return m_vram[(addr - 0x3000) % 0x0800];
	} else if (addr >= 0x3F00 && addr < 0x4000) {
		return m_palette[addr % 32];
	}
	return 0x00;
}

void NESPPU::write(Uint16 addr, Uint8 data) {
	if (addr >= 0x0000 && addr < 0x1000) {
		m_emu->m_dsk->chr_roms[m_emu->m_dsk->active_chr_rom[0]][addr % 0x1000] = data;
	} else if (addr >= 0x1000 && addr < 0x2000) {
		m_emu->m_dsk->chr_roms[m_emu->m_dsk->active_chr_rom[1]][addr % 0x1000] = data;
	} else if (addr >= 0x2000 && addr < 0x3000) {
		m_vram[(addr - 0x2000) % 0x0800] = data;
	} else if (addr >= 0x3000 && addr < 0x3F00) {
		m_vram[(addr - 0x3000) % 0x0800] = data;
	} else if (addr >= 0x3F00 && addr < 0x4000) {
		m_palette[addr % 32] = data;
	}
}
