#pragma once

class NESEMU;

class NESCMP {
public:
	NESCMP(NESEMU *emu) : m_emu(emu) {};
	~NESCMP() = default;

	void poweron();
	void reset();
	void update();

protected:
	friend class NESEMU;
	friend class NESBUS;
	friend class NESCPU;
	friend class NESPPU;
	friend class NESDSK;
	NESEMU *m_emu;
};