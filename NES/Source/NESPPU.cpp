#include "NESPPU.h"
#include "NESEMU.h"
#include "NESCPU.h"

NESPPU::NESPPU(NESEMU *emu) : NESCMP(emu), m_width(256), m_height(240) {
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
		"DOT: %03X, LINE: %03X, PPUCTRL: %02X, PPUMASK: %02X, PPUSTATUS: %02X", m_dot, m_scanline, m_emu->m_bus->read(0x2000), m_emu->m_bus->read(0x2001), m_emu->m_bus->read(0x2002));
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
		m_dot = 0;
		m_scanline++;
	}

	if (m_scanline == 241 && m_dot == 0) {
		// VBLANK
		m_registers[2] = m_registers[2] | 0x80;
		m_isVblank = true;
		m_emu->m_cpu->m_isNMIInterruptReq = true;
	}

	if (m_scanline >= 262) {
		// FRAME
		m_scanline = 0;
		m_dot = 0;
		m_registers[2] = m_registers[2] & ~0x80;
		m_isVblank = false;
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
