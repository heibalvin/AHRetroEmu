#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include "NESCMP.h"

#include <string>
using namespace std;

enum NESCPUAddressingMode {
	IMMEDIATE,
	ZERO_PAGE,
	ZERO_PAGE_X,
	ZERO_PAGE_Y,
	ABSOLUTE,
	ABSOLUTE_X,
	ABSOLUTE_Y,
	INDIRECT,
	INDEXED_INDIRECT,
	INDIRECT_INDEXED,
	RELATIVE,
	ACCUMULATOR,
	IMPLIED
};

struct NESCPUOPCODE {
	int opc = 0xFF;
	const char* mnemonic = "UNKNOWN";
	NESCPUAddressingMode mode = IMPLIED;
	int length = 1;
	int cycles = 1;
};

class NESCPU: NESCMP {
public:
	NESCPU(NESEMU *emu);
	~NESCPU();
	char* dump();

	void poweron();
	void update();

	void fetch();
	void decode();
	void execute();
	
	void testZeroFlag(Uint8 value);
	void testNegativeFlag(Uint8 value);
	Uint16 addressingMode();

	void pushWord(Uint16 value);
	Uint16 popWord();

	void pushByte(Uint8 value);
	Uint8 popByte();

	void CLC();
	void SEC();
	void CLI();
	void SEI();
	void CLV();
	void CLD();
	void SED();
	void TXS();
	void AND();
	void ORA();
	void EOR();
	void ASL();
	void LSR();
	void ROL();
	void ROR();
	void BIT();
	void LDA();
	void LDX();
	void LDY();
	void STA();
	void STX();
	void STY();
	void TAX();
	void TXA();
	void TAY();
	void TYA();
	void TSX();
	void ADC();
	void SBC();
	void INC();
	void DEC();
	void INX();
	void DEX();
	void INY();
	void DEY();
	void CMP();
	void CPX();
	void CPY();
	void BCC();
	void BCS();
	void BEQ();
	void BNE();
	void BPL();
	void BMI();
	void BVC();
	void BVS();
	void JMP();
	void JSR();
	void RTS();
	void BRK();
	void RTI();
	void PHP();
	void PLA();
	void PLP();
	void PHA();
	void NOP();

	void reset_interrupt();
	void nmi_interrupt();
	void irq_interrupt();

private:
	friend class NESBUS;
	friend class NESEMU;
	friend class NESPPU;

	// CPU state variables
	Uint16 PC; // Program Counter
	Uint8 SP;  // Stack Pointer
	Uint8 A;   // Accumulator
	Uint8 X;   // X Register
	Uint8 Y;   // Y Register
	Uint8 P;   // Processor Status

	Uint8 *m_wram;
	NESCPUOPCODE opcode_table[256]; // Opcode lookup table
	NESCPUOPCODE *opcode; // Currently executing opcode
	Uint16 nextPC; // Program Counter
	int delay_cycles = 0;

	bool m_isResetInterruptReq = false;
	bool m_isNMIInterruptReq = false;
	bool m_isIRQInterruptReq = false;
};