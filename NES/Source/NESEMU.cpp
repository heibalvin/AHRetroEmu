#include "NESEMU.h"
#include <SDL3/SDL.h>

NESEMU::NESEMU() : NESCMP(nullptr) {
	m_bus = new NESBUS(this);
	m_dsk = new NESDSK(this);
	m_cpu = new NESCPU(this);
	m_ppu = new NESPPU(this);
}

NESEMU::~NESEMU() {
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
	m_cycle_count = 0;
	m_cpu->poweron();
	m_ppu->poweron();

	// m_mode = RUN;
	m_mode = PAUSE;
	SDL_Log("NESEMU: poweron successfully - DEBUG PAUSE mode");
}

void NESEMU::step() {
	// Run one cycle
	m_ppu->update();
	if (m_cycle_count % 3 == 0) {
		m_cpu->update();
	}
	m_cycle_count += 1;
}

void NESEMU::update() {
	if (m_mode == PAUSE) {
		return;
	}

	step();
	if (m_isVBlankEvent == true) {
		m_isAppRefreshReq = true;
		m_isVBlankEvent = false;

		if (m_mode == VBLANK) {
			m_mode = PAUSE;
			SDL_Log("NESEMU: VBlank reached - PAUSED");
			return;
		}
	}

	// Check for line event
	if (m_mode == LINE && m_isLineEvent) {
		m_mode = PAUSE;
		m_isLineEvent = false;
		SDL_Log("NESEMU: Line complete - PAUSED");
		return;
	}

	if (m_mode == FRAME && m_isFrameEvent) {
		m_mode = PAUSE;
		m_isFrameEvent = false;
		SDL_Log("NESEMU: Frame complete - PAUSED");
		return;
	}

	if (m_mode == STEP) {
		m_mode = PAUSE;
		return;
	}
}

void NESEMU::defaultRunMode() {
	m_mode = RUN;
	SDL_Log("NESEMU: Running continuously");
}

void NESEMU::debugStepMode() {
	m_mode = STEP;
}

void NESEMU::debugLineMode() {
	m_mode = LINE;
	SDL_Log("NESEMU: Running until next line");
}

void NESEMU::debugVBlankMode() {
	m_mode = VBLANK;
	SDL_Log("NESEMU: Running until VBlank");
}

void NESEMU::debugFrameMode() {
	m_mode = FRAME;
	SDL_Log("NESEMU: Running until frame complete");
}

void NESEMU::loadRom(Uint8* datas) {
	m_dsk->load(datas);

	char* json = m_dsk->toJSON();
	SDL_Log("NESEMU: ROM loaded: %s", json);
	SDL_free(json);
}
