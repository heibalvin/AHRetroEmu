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
	SDL_Log("NESEMU: poweron successfully!");
}

void NESEMU::update() {
	m_ppu->update();
	m_ppu->update();
	m_ppu->update();

	m_cpu->update();
	
	m_cycle_count += 3;
}

void NESEMU::loadRom(Uint8* datas) {
	m_dsk->load(datas);

	char* json = m_dsk->toJSON();
	SDL_Log("NESEMU: ROM loaded: %s", json);
	SDL_free(json);
}