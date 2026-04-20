#include "NESEMU.h"
#include <SDL3/SDL.h>

NESEMU::NESEMU() : NESCMP(nullptr) {
	m_bus = new NESBUS(this);
	m_dsk = new NESDSK(this);
	m_cpu = new NESCPU(this);
	m_ppu = new NESPPU(this);
}

NESEMU::~NESEMU() {
	// Destructor implementation
	if (m_dsk) {
		delete m_dsk;
		m_dsk = nullptr;
	}
	if (m_cpu) {
		delete m_cpu;
		m_cpu = nullptr;
	}
	if (m_ppu) {
		delete m_ppu;
		m_ppu = nullptr;
	}
	if (m_bus) {
		delete m_bus;
		m_bus = nullptr;
	}
}

void NESEMU::poweron() {
	// Power on implementation
	m_cycle_count = 0;
	m_cpu->poweron();
	m_ppu->poweron();
	SDL_Log("NESEMU: emulator powered on");
}

void NESEMU::reset() {
	// Reset implementation
	m_cycle_count = 0;
	SDL_Log("NESEMU: emulator reset");
}

void NESEMU::update() {
	// Update implementation
	m_ppu->update();

	if (m_cycle_count % 3 == 0) {
		m_cpu->update();
	}
	
	m_cycle_count++;
	// SDL_Log("NESEMU: emulator state updated, cycle count: %d", m_cycle_count);
}

void NESEMU::loadRom(Uint8* datas) {
	// Load ROM implementation
	m_dsk->load(datas);
	char *json = m_dsk->toJSON();
	SDL_Log("NESEMU: ROM loaded: %s", json);
	delete[] json;
}