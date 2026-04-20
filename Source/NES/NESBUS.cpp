#include "NESBUS.h"
#include "NESEMU.h"

NESBUS::NESBUS(NESEMU *emu) : NESCMP(emu) {
	prg_ram = new Uint8[8 * 1024];
}

NESBUS::~NESBUS() {
	if (prg_ram) {
		delete[] prg_ram;
		prg_ram = nullptr;
	}
}

Uint8 NESBUS::read(Uint16 addr) {
	if (addr < 0x2000) {
		return m_emu->m_cpu->m_memory[addr % 0x0800];
	} else if (addr >= 0x2000 && addr < 0x4000) {
		return m_emu->m_ppu->m_registers[(addr - 0x2000) % 8];
	} else if (addr >= 0x4000 && addr < 0x4018) {
		SDL_Log("NESBUS: Reading from APU and I/O registers at address 0x%04X", addr);
		return 0x00;
	} else if (addr >= 0x4018 && addr < 0x4020) {
		SDL_Log("NESBUS: Reading from APU and I/O functionality at address 0x%04X", addr);
		return 0x00;
	} else if (addr >= 0x4020 && addr < 0x6000) {
		SDL_Log("NESBUS: Reading from expansion ROM at address 0x%04X", addr);
		return 0x00;
	} else if (addr >= 0x6000 && addr < 0x8000) {
		return prg_ram[addr - 0x6000];
	} else if (addr >= 0x8000 && addr < 0xC000) {
		return m_emu->m_dsk->prg_roms[m_emu->m_dsk->active_prg_rom[0]][addr % 0x4000];
	} else if (addr >= 0xC000 && addr <= 0xFFFF) {
		return m_emu->m_dsk->prg_roms[m_emu->m_dsk->active_prg_rom[1]][addr % 0x4000];
	}

	return 0x00;
}

void NESBUS::write(Uint16 addr, Uint8 data) {
	if (addr < 0x2000) {
		m_emu->m_cpu->m_memory[addr % 0x0800] = data;
	} else if (addr >= 0x2000 && addr < 0x4000) {
		m_emu->m_ppu->m_registers[(addr - 0x2000) % 8] = data;
	} else if (addr >= 0x4000 && addr < 0x4018) {
		SDL_Log("NESBUS: Writing to APU and I/O registers at address 0x%04X with data 0x%02X", addr, data);
	} else if (addr >= 0x4018 && addr < 0x4020) {
		SDL_Log("NESBUS: Writing to APU and I/O functionality at address 0x%04X with data 0x%02X", addr, data);
	} else if (addr >= 0x4020 && addr < 0x6000) {
		SDL_Log("NESBUS: Writing to expansion ROM at address 0x%04X with data 0x%02X", addr, data);
	} else if (addr >= 0x6000 && addr < 0x8000) {
		prg_ram[addr - 0x6000] = data;
	} else if (addr >= 0x8000 && addr < 0xC000) {
		m_emu->m_dsk->prg_roms[m_emu->m_dsk->active_prg_rom[0]][addr % 0x4000] = data;
	} else if (addr >= 0xC000 && addr <= 0xFFFF) {
		m_emu->m_dsk->prg_roms[m_emu->m_dsk->active_prg_rom[1]][addr % 0x4000] = data;
	}
}

Uint16 NESBUS::readLE(Uint16 addr) {
	Uint8 low = read(addr);
	Uint8 high = read(addr + 1);
	return (high << 8) | low;
}

void NESBUS::writeLE(Uint16 addr, Uint16 data) {
	Uint8 low = data & 0xFF;
	Uint8 high = (data >> 8) & 0xFF;
	write(addr, low);
	write(addr + 1, high);
}

Uint16 NESBUS::readBE(Uint16 addr) {
	Uint8 high = read(addr);
	Uint8 low = read(addr + 1);
	return (high << 8) | low;
}

void NESBUS::writeBE(Uint16 addr, Uint16 data) {
	Uint8 high = (data >> 8) & 0xFF;
	Uint8 low = data & 0xFF;
	write(addr, high);
	write(addr + 1, low);
}