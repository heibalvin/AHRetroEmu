#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include "NESCMP.h"
#include "NESDSK.h"
#include "NESCPU.h"
#include "NESPPU.h"
#include "NESBUS.h"

class NESEMU: NESCMP {
public:
	enum Mode {
		PAUSE,
		STEP,
		LINE,
		VBLANK,
		FRAME,
		RUN
	};

	NESEMU();
	~NESEMU();

	void poweron();
	void update();
	void step();

	void defaultRunMode();
	void debugStepMode();
	void debugLineMode();
	void debugVBlankMode();
	void debugFrameMode();

	void loadRom(Uint8* datas);

private:
	friend class SDLEMUAPP;
	friend class NESBUS;
	friend class NESCPU;
	friend class NESPPU;
	friend class NESDSK;
	NESBUS *m_bus;
	NESDSK *m_dsk;
	NESCPU *m_cpu;
	NESPPU *m_ppu;
	int m_cycle_count = 0;
	bool m_isLineEvent = false;
	bool m_isVBlankEvent = false;
	bool m_isFrameEvent = false;
	bool m_isStepEvent = false;

	bool m_isAppRefreshReq = false;
	Mode m_mode = PAUSE;
};
