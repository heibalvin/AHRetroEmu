#include "NESBUS.h"
#include "NESEMU.h"

NESBUS::NESBUS(NESEMU *emu) : NESCMP(emu) {
	m_apu_registers = (Uint8 *)SDL_malloc(22 * sizeof(Uint8));
	m_io_registers = (Uint8 *)SDL_malloc(2 * sizeof(Uint8));
}

NESBUS::~NESBUS() {
	if (m_apu_registers) {
		SDL_free(m_apu_registers);
		m_apu_registers = nullptr;
	}
	if (m_io_registers) {
		SDL_free(m_io_registers);
		m_io_registers = nullptr;
	}
}

Uint8 NESBUS::read(Uint16 addr) {
	if (addr < 0x2000) {
		return m_emu->m_cpu->m_wram[addr % 0x0800];
	} else if (addr >= 0x2000 && addr < 0x4000) {
		return m_emu->m_ppu->read((addr - 0x2000) % 8);
	} else if (addr >= 0x4000 && addr < 0x4016) {
		return m_apu_registers[(addr - 0x4000)];
	} else if (addr >= 0x4016 && addr < 0x4018) {
		return m_io_registers[(addr - 0x4016)];
	} else if (addr >= 0x4018 && addr < 0x4020) {
		SDL_Log("NESBUS: Invalid Read address bus 0x%04X", addr);
		return 0x00;
	}  else if (addr >= 0x4020 && addr < 0x6000) {
		SDL_Log("NESBUS: Reading from expansion ROM at address 0x%04X", addr);
		return 0x00;
	} else if (addr >= 0x6000 && addr < 0x8000) {
		SDL_Log("NESBUS: Reading from PRGRAM at address 0x%04X", addr);
		return 0x00;
	} else if (addr >= 0x8000 && addr < 0xC000) {
		return m_emu->m_dsk->prg_roms[m_emu->m_dsk->active_prg_rom[0]][addr % 0x4000];
	} else if (addr >= 0xC000 && addr <= 0xFFFF) {
		return m_emu->m_dsk->prg_roms[m_emu->m_dsk->active_prg_rom[1]][addr % 0x4000];
	}

	return 0x00;
}

void NESBUS::write(Uint16 addr, Uint8 data) {
	if (addr < 0x2000) {
		m_emu->m_cpu->m_wram[addr % 0x0800] = data;
	} else if (addr >= 0x2000 && addr < 0x4000) {
		m_emu->m_ppu->write((addr - 0x2000) % 8, data);
	} else if (addr >= 0x4000 && addr < 0x4016) {
		m_apu_registers[(addr - 0x4000)] = data;
	} else if (addr >= 0x4016 && addr < 0x4018) {
		m_io_registers[(addr - 0x4016)] = data;
	} else if (addr >= 0x4018 && addr < 0x4020) {
		SDL_Log("NESBUS: Invalid Write address bus 0x%04X with data 0x%02X", addr, data);
	} else if (addr >= 0x4020 && addr < 0x6000) {
		SDL_Log("NESBUS: Writing from expansion ROM at address 0x%04X with data 0x%02X", addr, data);
	} else if (addr >= 0x6000 && addr < 0x8000) {
		SDL_Log("NESBUS: Writing from PRGRAM at address 0x%04X", addr);
	} else if (addr >= 0x8000 && addr < 0xC000) {
		m_emu->m_dsk->prg_roms[m_emu->m_dsk->active_prg_rom[0]][addr % 0x4000] = data;
	} else if (addr >= 0xC000 && addr <= 0xFFFF) {
		m_emu->m_dsk->prg_roms[m_emu->m_dsk->active_prg_rom[1]][addr % 0x4000] = data;
	}
}

Uint16 NESBUS::readWord(Uint16 addr) {
	Uint8 low = read(addr);
	Uint8 high = read(addr + 1);
	return (high << 8) | low;
}

void NESBUS::writeWord(Uint16 addr, Uint16 data) {
	Uint8 low = data & 0xFF;
	Uint8 high = (data >> 8) & 0xFF;
	write(addr, low);
	write(addr + 1, high);
}
