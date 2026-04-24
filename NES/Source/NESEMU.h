#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include "NESCMP.h"
#include "NESDSK.h"
#include "NESCPU.h"
#include "NESPPU.h"
#include "NESBUS.h"

enum NESEMUMODE {
	NONE,
	DEFAULT,
	PAUSE,
	STEP,
	RST,
	IRQ,
	LINE,
	VBLANK,
	FRAME
};

class NESEMU: NESCMP {
public:
	

	NESEMU();
	~NESEMU();

	void poweron();
	void update();
	void step();

	void setMode(NESEMUMODE mode)	{	m_mode = mode;		};

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

	// NESEMU event
	bool m_isAppRefreshReq = false;
	
	// Mode m_mode = DEFAULT;
	NESEMUMODE m_mode = RST;
	NESEMUMODE m_event = NONE;
};
