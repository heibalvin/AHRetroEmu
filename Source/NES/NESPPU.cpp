#include "NESPPU.h"
#include "NESEMU.h"

NESPPU::NESPPU(NESEMU *emu) : NESCMP(emu) {
	memory = new Uint8[8];
}

NESPPU::~NESPPU() {
	// Destructor implementation
	if (memory) {
		delete[] memory;
		memory = nullptr;
	}
}

void NESPPU::poweron() {
	// Power on implementation
	for (int i = 0; i < 8; ++i) {
		memory[i] = 0x00; // Clear PPU registers
	}
}

void NESPPU::reset() {
	// Reset implementation
	for (int i = 0; i < 8; ++i) {
		memory[i] = 0x00; // Clear PPU registers
	}
}

void NESPPU::update() {
	// Update implementation
	// SDL_Log("NESPPU: Updating PPU state");
	// Render graphics and handle PPU operations here
}
