#include "NESDSK.h"
#include "NESEMU.h"

NESDSK::NESDSK(NESEMU *emu) : NESCMP(emu) {


}

NESDSK::~NESDSK() {
	// Destructor implementation
	if (prg_roms) {
		for (int i = 0; i < prg_rom_count; ++i) {
			delete[] prg_roms[i];
		}
		delete[] prg_roms;
	}
	if (chr_roms) {
		for (int i = 0; i < chr_rom_count; ++i) {
			delete[] chr_roms[i];
		}
		delete[] chr_roms;
	}
}

char* NESDSK::toJSON() {
	// Convert to JSON implementation
	char *json = new char[1024];
	SDL_snprintf(json, 1024, "{\"version\": %d, \"mapper\": %d, \"mirroring\": %d, \"tv_system\": %d, \"prg_rom_size\": %d, \"prg_rom_count\": %d, \"chr_rom_size\": %d, \"chr_rom_count\": %d}",
		version, mapper, mirroring, tv_system, prg_rom_size, prg_rom_count, chr_rom_size, chr_rom_count);

	return json;
}

void NESDSK::load(Uint8 *datas) {
	// Load ROM data into emulator
	if (datas[0] != 0x4E || datas[1] != 0x45 || datas[2] != 0x53 || datas[3] != 0x1A) {
		// Handle invalid ROM file
		SDL_Log("NESDSK: Invalid ROM file");
		return;
	}

	version = 1;
	if (datas[7] & 0x0C == 0x08) {
		version = 2;
	}
	mapper = (datas[6] >> 4) | (datas[7] & 0xF0);
	mirroring = datas[6] & 0x01;
	tv_system = datas[9] & 0x01;

	prg_rom_size = 16 * 1024; // 16 KB
	prg_rom_count = datas[4];
	prg_roms = new Uint8*[prg_rom_count];

	chr_rom_size = 8 * 1024; // 8 KB
	chr_rom_count = datas[5];
	chr_roms = new Uint8*[chr_rom_count];

	int addr = 0x0010; // Start of ROM data
	for (int i = 0; i < prg_rom_count; ++i) {
		prg_roms[i] = new Uint8[prg_rom_size];
		SDL_memcpy(prg_roms[i], &datas[addr], prg_rom_size);
		addr += prg_rom_size;
	}
	for (int i = 0; i < chr_rom_count; ++i) {
		chr_roms[i] = new Uint8[chr_rom_size];
		SDL_memcpy(chr_roms[i], &datas[addr], chr_rom_size);
		addr += chr_rom_size;
	}

	active_prg_rom[0] = 0; // Start with the first PRG ROM bank active
	active_prg_rom[1] = prg_rom_count > 1 ? 1 : 0; // If there's a second bank, use it; otherwise, mirror the first bank
	
	active_chr_rom[0] = 0; // Start with the first CHR ROM bank active
	active_chr_rom[1] = chr_rom_count > 1 ? 1 : 0; // If there's a second bank, use it; otherwise, mirror the first bank
}