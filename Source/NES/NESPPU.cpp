#include "NESPPU.h"
#include "NESEMU.h"

NESPPU::NESPPU(NESEMU *emu) : NESCMP(emu) {
	m_registers = new Uint8[8];
	m_vram = new Uint8[0x0800]; // 2KB of VRAM
	m_oam = new Uint8[256]; // 256 bytes of OAM
	m_palette = new Uint8[32]; // 32 bytes of palette RAM
}

NESPPU::~NESPPU() {
	// Destructor implementation
	if (m_registers) {
		delete[] m_registers;
		m_registers = nullptr;
	}
	if (m_vram) {
		delete[] m_vram;
		m_vram = nullptr;
	}
	if (m_oam) {
		delete[] m_oam;
		m_oam = nullptr;
	}
	if (m_palette) {
		delete[] m_palette;
		m_palette = nullptr;
	}
}

void NESPPU::poweron() {
	// Power on implementation
	for (int i = 0; i < 8; ++i) {
		m_registers[i] = 0x00; // Clear PPU registers
	}
}

void NESPPU::reset() {
	// Reset implementation
	for (int i = 0; i < 8; ++i) {
		m_registers[i] = 0x00; // Clear PPU registers
	}
}

void NESPPU::update() {
	// Update implementation
	SDL_Log("PPU Registers: %02X %02X %02X %02X %02X %02X %02X %02X",
		m_registers[0], m_registers[1], m_registers[2], m_registers[3],
		m_registers[4], m_registers[5], m_registers[6], m_registers[7]);
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

Uint16 NESPPU::readLE(Uint16 addr) {
	Uint8 low = read(addr);
	Uint8 high = read(addr + 1);
	return (high << 8) | low;
}

void NESPPU::writeLE(Uint16 addr, Uint16 data) {
	Uint8 low = data & 0xFF;
	Uint8 high = (data >> 8) & 0xFF;
	write(addr, low);
	write(addr + 1, high);
}