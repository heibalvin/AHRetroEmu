#include "NESCPU.h"
#include "NESEMU.h"

NESCPU::NESCPU(NESEMU *emu) : NESCMP(emu) {
	m_wram = (Uint8 *)SDL_malloc(2 * 1024 * sizeof(Uint8));

	// Interrupt flag opcodes
	opcode_table[0x18] = { 0x18, "CLC", IMPLIED, 1, 2 };
	opcode_table[0x38] = { 0x38, "SEC", IMPLIED, 1, 2 };
	opcode_table[0x58] = { 0x58, "CLI", IMPLIED, 1, 2 };
	opcode_table[0x78] = { 0x78, "SEI", IMPLIED, 1, 2 };
	opcode_table[0xB8] = { 0xB8, "CLV", IMPLIED, 1, 2 };
	opcode_table[0xD8] = { 0xD8, "CLD", IMPLIED, 1, 2 };
	opcode_table[0xF8] = { 0xF8, "SED", IMPLIED, 1, 2 };

	// Transfer opcodes
	opcode_table[0xAA] = { 0xAA, "TAX", IMPLIED, 1, 2 };
	opcode_table[0x8A] = { 0x8A, "TXA", IMPLIED, 1, 2 };
	opcode_table[0xA8] = { 0xA8, "TAY", IMPLIED, 1, 2 };
	opcode_table[0x98] = { 0x98, "TYA", IMPLIED, 1, 2 };
	opcode_table[0x9A] = { 0x9A, "TXS", IMPLIED, 1, 2 };
	opcode_table[0xBA] = { 0xBA, "TSX", IMPLIED, 1, 2 };
	opcode_table[0xA9] = { 0xA9, "LDA", IMMEDIATE, 2, 2 };
	opcode_table[0xA5] = { 0xA5, "LDA", ZERO_PAGE, 2, 3 };
	opcode_table[0xB5] = { 0xB5, "LDA", ZERO_PAGE_X, 2, 4 };
	opcode_table[0xAD] = { 0xAD, "LDA", ABSOLUTE, 3, 4 };
	opcode_table[0xBD] = { 0xBD, "LDA", ABSOLUTE_X, 3, 4 };
	opcode_table[0xB9] = { 0xB9, "LDA", ABSOLUTE_Y, 3, 4 };
	opcode_table[0xA1] = { 0xA1, "LDA", INDEXED_INDIRECT, 2, 6 };
	opcode_table[0xB1] = { 0xB1, "LDA", INDIRECT_INDEXED, 2, 5 };

	// LDX - Load X
	opcode_table[0xA2] = { 0xA2, "LDX", IMMEDIATE, 2, 2 };
	opcode_table[0xA6] = { 0xA6, "LDX", ZERO_PAGE, 2, 3 };
	opcode_table[0xB6] = { 0xB6, "LDX", ZERO_PAGE_Y, 2, 4 };
	opcode_table[0xAE] = { 0xAE, "LDX", ABSOLUTE, 3, 4 };
	opcode_table[0xBE] = { 0xBE, "LDX", ABSOLUTE_Y, 3, 4 };

	// LDY - Load Y
	opcode_table[0xA0] = { 0xA0, "LDY", IMMEDIATE, 2, 2 };
	opcode_table[0xA4] = { 0xA4, "LDY", ZERO_PAGE, 2, 3 };
	opcode_table[0xB4] = { 0xB4, "LDY", ZERO_PAGE_X, 2, 4 };
	opcode_table[0xAC] = { 0xAC, "LDY", ABSOLUTE, 3, 4 };
	opcode_table[0xBC] = { 0xBC, "LDY", ABSOLUTE_X, 3, 4 };

	// STA - Store A
	opcode_table[0x85] = { 0x85, "STA", ZERO_PAGE, 2, 3 };
	opcode_table[0x95] = { 0x95, "STA", ZERO_PAGE_X, 2, 4 };
	opcode_table[0x8D] = { 0x8D, "STA", ABSOLUTE, 3, 4 };
	opcode_table[0x9D] = { 0x9D, "STA", ABSOLUTE_X, 3, 5 };
	opcode_table[0x99] = { 0x99, "STA", ABSOLUTE_Y, 3, 5 };
	opcode_table[0x81] = { 0x81, "STA", INDEXED_INDIRECT, 2, 6 };
	opcode_table[0x91] = { 0x91, "STA", INDIRECT_INDEXED, 2, 6 };

	// STX - Store X
	opcode_table[0x86] = { 0x86, "STX", ZERO_PAGE, 2, 3 };
	opcode_table[0x96] = { 0x96, "STX", ZERO_PAGE_Y, 2, 4 };
	opcode_table[0x8E] = { 0x8E, "STX", ABSOLUTE, 3, 4 };

	// STY - Store Y
	opcode_table[0x84] = { 0x84, "STY", ZERO_PAGE, 2, 3 };
	opcode_table[0x94] = { 0x94, "STY", ZERO_PAGE_X, 2, 4 };
	opcode_table[0x8C] = { 0x8C, "STY", ABSOLUTE, 3, 4 };

	// Arithmetic opcodes - ADC
	opcode_table[0x69] = { 0x69, "ADC", IMMEDIATE, 2, 2 };
	opcode_table[0x65] = { 0x65, "ADC", ZERO_PAGE, 2, 3 };
	opcode_table[0x75] = { 0x75, "ADC", ZERO_PAGE_X, 2, 4 };
	opcode_table[0x6D] = { 0x6D, "ADC", ABSOLUTE, 3, 4 };
	opcode_table[0x7D] = { 0x7D, "ADC", ABSOLUTE_X, 3, 4 };
	opcode_table[0x79] = { 0x79, "ADC", ABSOLUTE_Y, 3, 4 };
	opcode_table[0x61] = { 0x61, "ADC", INDEXED_INDIRECT, 2, 6 };
	opcode_table[0x71] = { 0x71, "ADC", INDIRECT_INDEXED, 2, 5 };

	// Arithmetic opcodes - SBC
	opcode_table[0xE9] = { 0xE9, "SBC", IMMEDIATE, 2, 2 };
	opcode_table[0xE5] = { 0xE5, "SBC", ZERO_PAGE, 2, 3 };
	opcode_table[0xF5] = { 0xF5, "SBC", ZERO_PAGE_X, 2, 4 };
	opcode_table[0xED] = { 0xED, "SBC", ABSOLUTE, 3, 4 };
	opcode_table[0xFD] = { 0xFD, "SBC", ABSOLUTE_X, 3, 4 };
	opcode_table[0xF9] = { 0xF9, "SBC", ABSOLUTE_Y, 3, 4 };
	opcode_table[0xE1] = { 0xE1, "SBC", INDEXED_INDIRECT, 2, 6 };
	opcode_table[0xF1] = { 0xF1, "SBC", INDIRECT_INDEXED, 2, 5 };

	// Arithmetic opcodes - INC
	opcode_table[0xE6] = { 0xE6, "INC", ZERO_PAGE, 2, 5 };
	opcode_table[0xF6] = { 0xF6, "INC", ZERO_PAGE_X, 2, 6 };
	opcode_table[0xEE] = { 0xEE, "INC", ABSOLUTE, 3, 6 };
	opcode_table[0xFE] = { 0xFE, "INC", ABSOLUTE_X, 3, 7 };

	// Arithmetic opcodes - DEC
	opcode_table[0xC6] = { 0xC6, "DEC", ZERO_PAGE, 2, 5 };
	opcode_table[0xD6] = { 0xD6, "DEC", ZERO_PAGE_X, 2, 6 };
	opcode_table[0xCE] = { 0xCE, "DEC", ABSOLUTE, 3, 6 };
	opcode_table[0xDE] = { 0xDE, "DEC", ABSOLUTE_X, 3, 7 };

	// Arithmetic opcodes - INX, DEX, INY, DEY
	opcode_table[0xE8] = { 0xE8, "INX", IMPLIED, 1, 2 };
	opcode_table[0xCA] = { 0xCA, "DEX", IMPLIED, 1, 2 };
	opcode_table[0xC8] = { 0xC8, "INY", IMPLIED, 1, 2 };
	opcode_table[0x88] = { 0x88, "DEY", IMPLIED, 1, 2 };

	// Compare opcodes - CMP
	opcode_table[0xC9] = { 0xC9, "CMP", IMMEDIATE, 2, 2 };
	opcode_table[0xC5] = { 0xC5, "CMP", ZERO_PAGE, 2, 3 };
	opcode_table[0xD5] = { 0xD5, "CMP", ZERO_PAGE_X, 2, 4 };
	opcode_table[0xCD] = { 0xCD, "CMP", ABSOLUTE, 3, 4 };
	opcode_table[0xDD] = { 0xDD, "CMP", ABSOLUTE_X, 3, 4 };
	opcode_table[0xD9] = { 0xD9, "CMP", ABSOLUTE_Y, 3, 4 };
	opcode_table[0xC1] = { 0xC1, "CMP", INDEXED_INDIRECT, 2, 6 };
	opcode_table[0xD1] = { 0xD1, "CMP", INDIRECT_INDEXED, 2, 5 };

	// Compare opcodes - CPX
	opcode_table[0xE0] = { 0xE0, "CPX", IMMEDIATE, 2, 2 };
	opcode_table[0xE4] = { 0xE4, "CPX", ZERO_PAGE, 2, 3 };
	opcode_table[0xEC] = { 0xEC, "CPX", ABSOLUTE, 3, 4 };

	// Compare opcodes - CPY
	opcode_table[0xC0] = { 0xC0, "CPY", IMMEDIATE, 2, 2 };
	opcode_table[0xC4] = { 0xC4, "CPY", ZERO_PAGE, 2, 3 };
	opcode_table[0xCC] = { 0xCC, "CPY", ABSOLUTE, 3, 4 };

	// Branch opcodes
	opcode_table[0x90] = { 0x90, "BCC", RELATIVE, 2, 2 };
	opcode_table[0xB0] = { 0xB0, "BCS", RELATIVE, 2, 2 };
	opcode_table[0xF0] = { 0xF0, "BEQ", RELATIVE, 2, 2 };
	opcode_table[0xD0] = { 0xD0, "BNE", RELATIVE, 2, 2 };
	opcode_table[0x10] = { 0x10, "BPL", RELATIVE, 2, 2 };
	opcode_table[0x30] = { 0x30, "BMI", RELATIVE, 2, 2 };
	opcode_table[0x50] = { 0x50, "BVC", RELATIVE, 2, 2 };
	opcode_table[0x70] = { 0x70, "BVS", RELATIVE, 2, 2 };

	// Jump opcodes
	opcode_table[0x4C] = { 0x4C, "JMP", ABSOLUTE, 3, 3 };
	opcode_table[0x6C] = { 0x6C, "JMP", INDIRECT, 3, 5 };
	opcode_table[0x20] = { 0x20, "JSR", ABSOLUTE, 3, 6 };
	opcode_table[0x60] = { 0x60, "RTS", IMPLIED, 1, 6 };
	opcode_table[0x00] = { 0x00, "BRK", IMPLIED, 1, 7 };
	opcode_table[0x40] = { 0x40, "RTI", IMPLIED, 1, 6 };

	// Bitwise opcodes - AND
	opcode_table[0x29] = { 0x29, "AND", IMMEDIATE, 2, 2 };
	opcode_table[0x25] = { 0x25, "AND", ZERO_PAGE, 2, 3 };
	opcode_table[0x35] = { 0x35, "AND", ZERO_PAGE_X, 2, 4 };
	opcode_table[0x2D] = { 0x2D, "AND", ABSOLUTE, 3, 4 };
	opcode_table[0x3D] = { 0x3D, "AND", ABSOLUTE_X, 3, 4 };
	opcode_table[0x39] = { 0x39, "AND", ABSOLUTE_Y, 3, 4 };
	opcode_table[0x21] = { 0x21, "AND", INDEXED_INDIRECT, 2, 6 };
	opcode_table[0x31] = { 0x31, "AND", INDIRECT_INDEXED, 2, 5 };

	// Bitwise opcodes - ORA
	opcode_table[0x09] = { 0x09, "ORA", IMMEDIATE, 2, 2 };
	opcode_table[0x05] = { 0x05, "ORA", ZERO_PAGE, 2, 3 };
	opcode_table[0x15] = { 0x15, "ORA", ZERO_PAGE_X, 2, 4 };
	opcode_table[0x0D] = { 0x0D, "ORA", ABSOLUTE, 3, 4 };
	opcode_table[0x1D] = { 0x1D, "ORA", ABSOLUTE_X, 3, 4 };
	opcode_table[0x19] = { 0x19, "ORA", ABSOLUTE_Y, 3, 4 };
	opcode_table[0x01] = { 0x01, "ORA", INDEXED_INDIRECT, 2, 6 };
	opcode_table[0x11] = { 0x11, "ORA", INDIRECT_INDEXED, 2, 5 };

	// Bitwise opcodes - EOR
	opcode_table[0x49] = { 0x49, "EOR", IMMEDIATE, 2, 2 };
	opcode_table[0x45] = { 0x45, "EOR", ZERO_PAGE, 2, 3 };
	opcode_table[0x55] = { 0x55, "EOR", ZERO_PAGE_X, 2, 4 };
	opcode_table[0x4D] = { 0x4D, "EOR", ABSOLUTE, 3, 4 };
	opcode_table[0x5D] = { 0x5D, "EOR", ABSOLUTE_X, 3, 4 };
	opcode_table[0x59] = { 0x59, "EOR", ABSOLUTE_Y, 3, 4 };
	opcode_table[0x41] = { 0x41, "EOR", INDEXED_INDIRECT, 2, 6 };
	opcode_table[0x51] = { 0x51, "EOR", INDIRECT_INDEXED, 2, 5 };

	// Bitwise opcodes - ASL
	opcode_table[0x0A] = { 0x0A, "ASL", ACCUMULATOR, 1, 2 };
	opcode_table[0x06] = { 0x06, "ASL", ZERO_PAGE, 2, 5 };
	opcode_table[0x16] = { 0x16, "ASL", ZERO_PAGE_X, 2, 6 };
	opcode_table[0x0E] = { 0x0E, "ASL", ABSOLUTE, 3, 6 };
	opcode_table[0x1E] = { 0x1E, "ASL", ABSOLUTE_X, 3, 7 };

	// Bitwise opcodes - LSR
	opcode_table[0x4A] = { 0x4A, "LSR", ACCUMULATOR, 1, 2 };
	opcode_table[0x46] = { 0x46, "LSR", ZERO_PAGE, 2, 5 };
	opcode_table[0x56] = { 0x56, "LSR", ZERO_PAGE_X, 2, 6 };
	opcode_table[0x4E] = { 0x4E, "LSR", ABSOLUTE, 3, 6 };
	opcode_table[0x5E] = { 0x5E, "LSR", ABSOLUTE_X, 3, 7 };

	// Bitwise opcodes - ROL
	opcode_table[0x2A] = { 0x2A, "ROL", ACCUMULATOR, 1, 2 };
	opcode_table[0x26] = { 0x26, "ROL", ZERO_PAGE, 2, 5 };
	opcode_table[0x36] = { 0x36, "ROL", ZERO_PAGE_X, 2, 6 };
	opcode_table[0x2E] = { 0x2E, "ROL", ABSOLUTE, 3, 6 };
	opcode_table[0x3E] = { 0x3E, "ROL", ABSOLUTE_X, 3, 7 };

	// Bitwise opcodes - ROR
	opcode_table[0x6A] = { 0x6A, "ROR", ACCUMULATOR, 1, 2 };
	opcode_table[0x66] = { 0x66, "ROR", ZERO_PAGE, 2, 5 };
	opcode_table[0x76] = { 0x76, "ROR", ZERO_PAGE_X, 2, 6 };
	opcode_table[0x6E] = { 0x6E, "ROR", ABSOLUTE, 3, 6 };
	opcode_table[0x7E] = { 0x7E, "ROR", ABSOLUTE_X, 3, 7 };

	// Bitwise opcodes - BIT
	opcode_table[0x24] = { 0x24, "BIT", ZERO_PAGE, 2, 3 };
	opcode_table[0x2C] = { 0x2C, "BIT", ABSOLUTE, 3, 4 };

	// Stack opcodes - PHA, PLA, PHP, PLP
	opcode_table[0x48] = { 0x48, "PHA", IMPLIED, 1, 3 };
	opcode_table[0x68] = { 0x68, "PLA", IMPLIED, 1, 4 };
	opcode_table[0x08] = { 0x08, "PHP", IMPLIED, 1, 3 };
	opcode_table[0x28] = { 0x28, "PLP", IMPLIED, 1, 4 };

	// NOP
	opcode_table[0xEA] = { 0xEA, "NOP", IMPLIED, 1, 2 };

	// Undocumented NOPs
	opcode_table[0x1A] = { 0x1A, "NOP", IMPLIED, 1, 2 };
	opcode_table[0x3A] = { 0x3A, "NOP", IMPLIED, 1, 2 };
	opcode_table[0x5A] = { 0x5A, "NOP", IMPLIED, 1, 2 };
	opcode_table[0x7A] = { 0x7A, "NOP", IMPLIED, 1, 2 };
	opcode_table[0xDA] = { 0xDA, "NOP", IMPLIED, 1, 2 };
	opcode_table[0xFA] = { 0xFA, "NOP", IMPLIED, 1, 2 };

	// INC/DEC with ZPG,X and ABS,X variants
	opcode_table[0xD6] = { 0xD6, "DEC", ZERO_PAGE_X, 2, 6 };
	opcode_table[0xDE] = { 0xDE, "DEC", ABSOLUTE_X, 3, 7 };

	// LDA/ZPG,X
	opcode_table[0xB5] = { 0xB5, "LDA", ZERO_PAGE_X, 2, 4 };

	// STA ZPG,X
	opcode_table[0x95] = { 0x95, "STA", ZERO_PAGE_X, 2, 4 };

	opcode = nullptr;
}

NESCPU::~NESCPU() {
	if (m_wram) {
		SDL_free(m_wram);
		m_wram = nullptr;
	}
}

char* NESCPU::dump() {
	char *str = (char *)SDL_malloc(sizeof(char)*80);
	SDL_snprintf(str, 80,
		"A: %02X X: %02X Y: %02X SP: %02X P: %s%s%s%s%s%s%s%s PC: %04X",
		A, X, Y, SP, (P & 0x80) ? "N" : ".", 
		(P & 0x40) ? "V" : ".", 
		(P & 0x20) ? "-" : ".", 
		(P & 0x10) ? "B" : ".", 
		(P & 0x08) ? "D" : ".", 
		(P & 0x04) ? "I" : ".", 
		(P & 0x02) ? "Z" : ".", 
		(P & 0x01) ? "C" : ".", PC);
	return str;
}

void NESCPU::pushWord(Uint16 value) {
	m_wram[0x0100 + SP] = (value >> 8) & 0xFF;
	SP = (SP - 1) & 0xFF;
	m_wram[0x0100 + SP] = value & 0xFF;
	SP = (SP - 1) & 0xFF;
}

Uint16 NESCPU::popWord() {
	SP = (SP + 1) & 0XFF;
	Uint8 low = m_wram[0x0100 + SP];
	SP = (SP + 1) & 0XFF;
	Uint8 high = m_wram[0x0100 + SP];
	return (high << 8) | low;
}

void NESCPU::pushByte(Uint8 value) {
	m_wram[0x0100 + SP] = value;
	SP = (SP - 1) & 0xFF;
}

Uint8 NESCPU::popByte() {
	SP = (SP + 1) & 0xFF;
	return 	m_wram[0x0100 + SP];
}

void NESCPU::poweron() {
	A = 0x00;
	X = 0x00;
	Y = 0x00;
	SP = 0xFF;
	P = 0x24;
	PC = 0x0000;
	nextPC = PC;
	delay_cycles = 0;

	m_isRSTReq = true;

	char *str = dump();
	SDL_Log("NESCPU: %s | poweron", str);
	SDL_free(str);
}

void NESCPU::reset_interrupt() {
	P = (P & 0x04) | 0x04;
	SP = 0xFF;

	pushWord(PC);
	pushByte(P);

	PC = m_emu->m_bus->readWord(0xFFFC); // Reset vector will be loaded here
	nextPC = PC; // Initialize nextPC to the reset vector
	delay_cycles = 0; // Clear delay cycles

	char *str = dump();
	SDL_Log("NESCPU: %s | reset interrupt", str);
	SDL_free(str);
}

void NESCPU::nmi_interrupt() {
	P = (P & 0x04) | 0x20;
	pushWord(PC);
	pushByte(P);

	PC = m_emu->m_bus->readWord(0xFFFA); // NMI vector will be loaded here
		
	char *str = dump();
	SDL_Log("NESCPU: %s | nmi interrupt", str);
	SDL_free(str);
}

void NESCPU::irq_interrupt() {
	if ((P & 0x04) == 0) {
		P = (P & 0x04) | 0x20;
		pushWord(PC);
		pushByte(P);

		PC = m_emu->m_bus->readWord(0xFFFE); // IRQ vector will be loaded here

		char *str = dump();
		SDL_Log("NESCPU: %s | irq interrupt", str);
		SDL_free(str);
	}
}

void NESCPU::update() {
	/* DEBUG: removed delay cycle for debugging
	if (delay_cycles > 0) {
		delay_cycles --;
		SDL_Log("NESCPU: Delay Cycle");
		return;
	}
	*/

	if (m_isRSTReq) {
		reset_interrupt();
		m_isRSTReq = false;
		m_emu->m_event = RST;
	}

	if (m_isNMIReq) {
		nmi_interrupt();
		m_isNMIReq = false;
	}

	if (m_isIRQReq) {
		irq_interrupt();
		m_isIRQReq = false;
		m_emu->m_event = IRQ;
	}

	fetch();
	decode();
	execute();
}

void NESCPU::fetch() {
	// Fetch implementation
	opcode = &opcode_table[m_emu->m_bus->read(PC)];
}

void NESCPU::decode() {
	Uint8 op = m_emu->m_bus->read(PC);
	Uint8 op1 = m_emu->m_bus->read(PC + 1);
	Uint8 op2 = m_emu->m_bus->read(PC + 2);
	
	char *registers = dump();

	char opcodes[80];
	if (opcode->length == 1) {
		SDL_snprintf(opcodes, 80, "%02X      ", op);
	} else if (opcode->length == 2) {
		SDL_snprintf(opcodes, 80, "%02X %02X   ", op, op1);
	} else if (opcode->length == 3) {
		SDL_snprintf(opcodes, 80, "%02X %02X %02X", op, op1, op2);
	}

	char mnemonic[80];
	switch (opcode->length) {
		case 1:
			SDL_snprintf(mnemonic, 80, "%s", opcode->mnemonic);
			break;
		case 2:
			switch (opcode->mode) {
				case IMMEDIATE:
					SDL_snprintf(mnemonic, 80, "%s #$%02X", opcode->mnemonic, op1);
					break;
				case ZERO_PAGE:
					SDL_snprintf(mnemonic, 80, "%s $%04X", opcode->mnemonic, op1);
					break;
				case ZERO_PAGE_X:
					SDL_snprintf(mnemonic, 80, "%s $%04X, X", opcode->mnemonic, op1);
					break;
				case ZERO_PAGE_Y:
					SDL_snprintf(mnemonic, 80, "%s $%04X, Y", opcode->mnemonic, op1);
					break;
				case RELATIVE:
					{
						int8_t offset = (int8_t)op1;
						SDL_snprintf(mnemonic, 80, "%s %+d", opcode->mnemonic, offset);
					}
					break;
				case INDEXED_INDIRECT:
					SDL_snprintf(mnemonic, 80, "%s ($%02X,X)", opcode->mnemonic, op1);
					break;
				case INDIRECT_INDEXED:
					SDL_snprintf(mnemonic, 80, "%s ($%02X),Y", opcode->mnemonic, op1);
					break;
				default:
					SDL_snprintf(mnemonic, 80, "%s", opcode->mnemonic);
					break;
			}
			break;
		case 3:
			{
				Uint16 oper = (op2 << 8) | op1;
				switch (opcode->mode) {
					case ABSOLUTE:
						SDL_snprintf(mnemonic, 80, "%s $%04X", opcode->mnemonic, oper);
						break;
					case ABSOLUTE_X:
						SDL_snprintf(mnemonic, 80, "%s $%04X,X", opcode->mnemonic, oper);
						break;
					case ABSOLUTE_Y:
						SDL_snprintf(mnemonic, 80, "%s $%04X,Y", opcode->mnemonic, oper);
						break;
					case INDIRECT:
						SDL_snprintf(mnemonic, 80, "%s ($%04X)", opcode->mnemonic, oper);
						break;
					default:
						SDL_snprintf(mnemonic, 80, "%s", opcode->mnemonic);
						break;
				}
			}
			break;
		default:
			SDL_snprintf(mnemonic, 80, "%04X : Decode Error", PC);
			break;
	}
	
	SDL_Log("NESCPU: %s | %s | %s", registers, opcodes, mnemonic);
	SDL_free(registers);
}

void NESCPU::execute() {
	nextPC = PC + opcode->length;

	switch (opcode->opc) {
		// Interrupt flag opcodes
		case 0x18: CLC(); break;
		case 0x38: SEC(); break;
		case 0x58: CLI(); break;
		case 0x78: SEI(); break;
		case 0xB8: CLV(); break;
		case 0xD8: CLD(); break;
		case 0xF8: SED(); break;

		// Bitwise opcodes - AND
		case 0x29:
		case 0x25:
		case 0x35:
		case 0x2D:
		case 0x3D:
		case 0x39:
		case 0x21:
		case 0x31: AND(); break;

		// Bitwise opcodes - ORA
		case 0x09:
		case 0x05:
		case 0x15:
		case 0x0D:
		case 0x1D:
		case 0x19:
		case 0x01:
		case 0x11: ORA(); break;

		// Bitwise opcodes - EOR
		case 0x49:
		case 0x45:
		case 0x55:
		case 0x4D:
		case 0x5D:
		case 0x59:
		case 0x41:
		case 0x51: EOR(); break;

		// Bitwise opcodes - ASL
		case 0x0A:
		case 0x06:
		case 0x16:
		case 0x0E:
		case 0x1E: ASL(); break;

		// Bitwise opcodes - LSR
		case 0x4A:
		case 0x46:
		case 0x56:
		case 0x4E:
		case 0x5E: LSR(); break;

		// Bitwise opcodes - ROL
		case 0x2A:
		case 0x26:
		case 0x36:
		case 0x2E:
		case 0x3E: ROL(); break;

		// Bitwise opcodes - ROR
		case 0x6A:
		case 0x66:
		case 0x76:
		case 0x6E:
		case 0x7E: ROR(); break;

		// Bitwise opcodes - BIT
		case 0x24:
		case 0x2C: BIT(); break;

		// Other opcodes
		case 0x9A: TXS(); break;

		// LDA - Load A
		case 0xA9:
		case 0xA5:
		case 0xB5:
		case 0xAD:
		case 0xBD:
		case 0xB9:
		case 0xA1:
		case 0xB1: LDA(); break;

		// LDX - Load X
		case 0xA2:
		case 0xA6:
		case 0xB6:
		case 0xAE:
		case 0xBE: LDX(); break;

		// LDY - Load Y
		case 0xA0:
		case 0xA4:
		case 0xB4:
		case 0xAC:
		case 0xBC: LDY(); break;

		// STA - Store A
		case 0x85:
		case 0x95:
		case 0x8D:
		case 0x9D:
		case 0x99:
		case 0x81:
		case 0x91: STA(); break;

		// STX - Store X
		case 0x86:
		case 0x96:
		case 0x8E: STX(); break;

		// STY - Store Y
		case 0x84:
		case 0x94:
		case 0x8C: STY(); break;

		// Transfer opcodes
		case 0xAA: TAX(); break;
		case 0x8A: TXA(); break;
		case 0xA8: TAY(); break;
		case 0x98: TYA(); break;
		case 0xBA: TSX(); break;

		// Arithmetic opcodes - ADC
		case 0x69:
		case 0x65:
		case 0x75:
		case 0x6D:
		case 0x7D:
		case 0x79:
		case 0x61:
		case 0x71: ADC(); break;

		// Arithmetic opcodes - SBC
		case 0xE9:
		case 0xE5:
		case 0xF5:
		case 0xED:
		case 0xFD:
		case 0xF9:
		case 0xE1:
		case 0xF1: SBC(); break;

		// Arithmetic opcodes - INC
		case 0xE6:
		case 0xF6:
		case 0xEE:
		case 0xFE: INC(); break;

		// Arithmetic opcodes - DEC
		case 0xC6:
		case 0xD6:
		case 0xCE:
		case 0xDE: DEC(); break;

		// Arithmetic opcodes - INX, DEX, INY, DEY
		case 0xE8: INX(); break;
		case 0xCA: DEX(); break;
		case 0xC8: INY(); break;
		case 0x88: DEY(); break;

		// Compare opcodes - CMP
		case 0xC9:
		case 0xC5:
		case 0xD5:
		case 0xCD:
		case 0xDD:
		case 0xD9:
		case 0xC1:
		case 0xD1: CMP(); break;

		// Compare opcodes - CPX
		case 0xE0:
		case 0xE4:
		case 0xEC: CPX(); break;

		// Compare opcodes - CPY
		case 0xC0:
		case 0xC4:
		case 0xCC: CPY(); break;

		// Branch opcodes
		case 0x90: BCC(); break;
		case 0xB0: BCS(); break;
		case 0xF0: BEQ(); break;
		case 0xD0: BNE(); break;
		case 0x10: BPL(); break;
		case 0x30: BMI(); break;
		case 0x50: BVC(); break;
		case 0x70: BVS(); break;

		// Jump opcodes
		case 0x4C:
		case 0x6C: JMP(); break;
		case 0x20: JSR(); break;
		case 0x60: RTS(); break;
		case 0x00: BRK(); break;
		case 0x40: RTI(); break;

		default:
			SDL_Log("NESCPU: Unknown opcode %02X at %04X", opcode->opc, PC);
			break;
	}

	delay_cycles = opcode->cycles - 1;
	PC = nextPC;
}

void NESCPU::testZeroFlag(Uint8 value) {
	if (value == 0) {
		P |= 0x02; // Set Zero flag
	} else {
		P &= ~0x02; // Clear Zero flag
	}
}

void NESCPU::testNegativeFlag(Uint8 value) {
	if (value & 0x80) {
		P |= 0x80; // Set Negative flag
	} else {
		P &= ~0x80; // Clear Negative flag
	}
}

Uint16 NESCPU::addressingMode() {
	switch (opcode->mode) {
		case IMMEDIATE:
			return PC + 1; // Immediate value is the next byte after the opcode
		case ZERO_PAGE:
			return (Uint16)(m_emu->m_bus->read(PC + 1));
		case ZERO_PAGE_X:
			return (Uint16)(m_emu->m_bus->read(PC + 1) + X) & 0x00FF;
		case ZERO_PAGE_Y:
			return (Uint16)(m_emu->m_bus->read(PC + 1) + Y) & 0x00FF;;
		case ABSOLUTE:
			return m_emu->m_bus->readWord(PC + 1);
		case ABSOLUTE_X:
			return m_emu->m_bus->readWord(PC + 1) + X;
		case ABSOLUTE_Y:
			return m_emu->m_bus->readWord(PC + 1) + Y;
		case INDIRECT:
			return m_emu->m_bus->readWord(m_emu->m_bus->readWord(PC + 1));
		case INDEXED_INDIRECT:
			return m_emu->m_bus->readWord((m_emu->m_bus->read(PC + 1) + X) & 0xFF);
		case INDIRECT_INDEXED:
			return m_emu->m_bus->readWord(m_emu->m_bus->read(PC + 1)) + Y;
		case RELATIVE:
			return PC + 2 + (int8_t)m_emu->m_bus->read(PC + 1); // Relative addressing uses signed offset
		case ACCUMULATOR:
		case IMPLIED:
			return 0; // No address needed for these modes
		default:
			SDL_Log("NESCPU: Addressing Mode Error: Unknown addressing mode for opcode %02X at %04X", opcode->opc, PC);
			break;
	}
}

void NESCPU::SEI() {
	P |= 0x04; // Set the Interrupt Disable flag
}

void NESCPU::CLI() {
	P &= ~0x04; // Clear the Interrupt Disable flag
}

void NESCPU::CLC() {
	P &= ~0x01; // Clear Carry flag
}

void NESCPU::SEC() {
	P |= 0x01; // Set Carry flag
}

void NESCPU::CLV() {
	P &= ~0x40; // Clear Overflow flag
}

void NESCPU::CLD() {
	P &= ~0x08; // Clear Decimal flag (TBD: Partially Implemented - decimal mode not fully emulated)
}

void NESCPU::SED() {
	P |= 0x08; // Set Decimal flag (TBD: Partially Implemented - decimal mode not fully emulated)
}

void NESCPU::TXS() {
	SP = X; // Transfer X register to Stack Pointer
}

void NESCPU::LDX() {
	Uint16 addr = addressingMode();
	Uint8 byte = m_emu->m_bus->read(addr);
	X = byte;
	testZeroFlag(X);
	testNegativeFlag(X);
}

void NESCPU::LDA() {
	Uint16 addr = addressingMode();
	Uint8 byte = m_emu->m_bus->read(addr);
	A = byte;
	testZeroFlag(A);
	testNegativeFlag(A);
}

void NESCPU::LDY() {
	Uint16 addr = addressingMode();
	Uint8 byte = m_emu->m_bus->read(addr);
	Y = byte;
	testZeroFlag(Y);
	testNegativeFlag(Y);
}

void NESCPU::STA() {
	Uint16 addr = addressingMode();
	m_emu->m_bus->write(addr, A);
}

void NESCPU::STX() {
	Uint16 addr = addressingMode();
	m_emu->m_bus->write(addr, X);
}

void NESCPU::STY() {
	Uint16 addr = addressingMode();
	m_emu->m_bus->write(addr, Y);
}

void NESCPU::TAX() {
	X = A;
	testZeroFlag(X);
	testNegativeFlag(X);
}

void NESCPU::TXA() {
	A = X;
	testZeroFlag(A);
	testNegativeFlag(A);
}

void NESCPU::TAY() {
	Y = A;
	testZeroFlag(Y);
	testNegativeFlag(Y);
}

void NESCPU::TYA() {
	A = Y;
	testZeroFlag(A);
	testNegativeFlag(A);
}

void NESCPU::TSX() {
	X = SP;
	testZeroFlag(X);
	testNegativeFlag(X);
}

void NESCPU::ADC() {
	Uint16 addr = addressingMode();
	Uint8 value = m_emu->m_bus->read(addr);
	Uint16 result = A + value + (P & 0x01);
	P = (P & 0x7E) | ((result >> 8) & 0x01);
	A = result & 0xFF;
	testZeroFlag(A);
	testNegativeFlag(A);
}

void NESCPU::SBC() {
	Uint16 addr = addressingMode();
	Uint8 value = m_emu->m_bus->read(addr);
	Uint16 result = A - value - (1 - (P & 0x01));
	P = (P & 0x7E) | (((result >> 8) & 0x01) ^ 0x01);
	A = result & 0xFF;
	testZeroFlag(A);
	testNegativeFlag(A);
}

void NESCPU::INC() {
	Uint16 addr = addressingMode();
	Uint8 value = m_emu->m_bus->read(addr);
	value = (value + 1) & 0xFF;
	m_emu->m_bus->write(addr, value);
	testZeroFlag(value);
	testNegativeFlag(value);
}

void NESCPU::DEC() {
	Uint16 addr = addressingMode();
	Uint8 value = m_emu->m_bus->read(addr);
	value = (value - 1) & 0xFF;
	m_emu->m_bus->write(addr, value);
	testZeroFlag(value);
	testNegativeFlag(value);
}

void NESCPU::INX() {
	X = (X + 1) & 0xFF;
	testZeroFlag(X);
	testNegativeFlag(X);
}

void NESCPU::DEX() {
	X = (X - 1) & 0xFF;
	testZeroFlag(X);
	testNegativeFlag(X);
}

void NESCPU::INY() {
	Y = (Y + 1) & 0xFF;
	testZeroFlag(Y);
	testNegativeFlag(Y);
}

void NESCPU::DEY() {
	Y = (Y - 1) & 0xFF;
	testZeroFlag(Y);
	testNegativeFlag(Y);
}

void NESCPU::CMP() {
	Uint16 addr = addressingMode();
	Uint8 value = m_emu->m_bus->read(addr);
	Uint8 result = A - value;
	P = (P & 0x7C) | ((result >> 7) & 0x80) | ((result == 0) ? 0x02 : 0) | ((A < value) ? 0x01 : 0);
}

void NESCPU::CPX() {
	Uint16 addr = addressingMode();
	Uint8 value = m_emu->m_bus->read(addr);
	Uint8 result = X - value;
	P = (P & 0x7C) | ((result >> 7) & 0x80) | ((result == 0) ? 0x02 : 0) | ((X < value) ? 0x01 : 0);
}

void NESCPU::CPY() {
	Uint16 addr = addressingMode();
	Uint8 value = m_emu->m_bus->read(addr);
	Uint8 result = Y - value;
	P = (P & 0x7C) | ((result >> 7) & 0x80) | ((result == 0) ? 0x02 : 0) | ((Y < value) ? 0x01 : 0);
}

void NESCPU::BCC() {
	Sint8 offset = (Sint8)m_emu->m_bus->read(PC + 1);
	if ((P & 0x01) == 0) {
		nextPC = (PC + 2 + offset) & 0xFFFF;
	}
}

void NESCPU::BCS() {
	Sint8 offset = (Sint8)m_emu->m_bus->read(PC + 1);
	if ((P & 0x01) != 0) {
		nextPC = (PC + 2 + offset) & 0xFFFF;
	}
}

void NESCPU::BEQ() {
	Sint8 offset = (Sint8)m_emu->m_bus->read(PC + 1);
	if ((P & 0x02) != 0) {
		nextPC = (PC + 2 + offset) & 0xFFFF;
	}
}

void NESCPU::BNE() {
	Sint8 offset = (Sint8)m_emu->m_bus->read(PC + 1);
	if ((P & 0x02) == 0) {
		nextPC = (PC + 2 + offset) & 0xFFFF;
	}
}

void NESCPU::BPL() {
	Sint8 offset = (Sint8)m_emu->m_bus->read(PC + 1);
	if ((P & 0x80) == 0) {
		nextPC = (PC + 2 + offset) & 0xFFFF;
	}
}

void NESCPU::BMI() {
	Sint8 offset = (Sint8)m_emu->m_bus->read(PC + 1);
	if ((P & 0x80) != 0) {
		nextPC = (PC + 2 + offset) & 0xFFFF;
	}
}

void NESCPU::BVC() {
	Sint8 offset = (Sint8)m_emu->m_bus->read(PC + 1);
	if ((P & 0x40) == 0) {
		nextPC = (PC + 2 + offset) & 0xFFFF;
	}
}

void NESCPU::BVS() {
	Sint8 offset = (Sint8)m_emu->m_bus->read(PC + 1);
	if ((P & 0x40) != 0) {
		nextPC = (PC + 2 + offset) & 0xFFFF;
	}
}

void NESCPU::JMP() {
	nextPC = addressingMode();
}

void NESCPU::JSR() {
	pushWord((PC + 2) & 0xFFFF);
	nextPC = addressingMode();
}

void NESCPU::RTS() {
	nextPC = popWord();
}

void NESCPU::BRK() {
	pushWord((PC + 2) & 0xFFFF);

	nextPC = m_emu->m_bus->readWord(0xFFFE);
	P |= 0x10;
}

void NESCPU::RTI() {
	P = popByte();
	nextPC = popWord();
}

void NESCPU::AND() {
	Uint16 addr = addressingMode();
	A = A & m_emu->m_bus->read(addr);
	testZeroFlag(A);
	testNegativeFlag(A);
}

void NESCPU::ORA() {
	Uint16 addr = addressingMode();
	A = A | m_emu->m_bus->read(addr);
	testZeroFlag(A);
	testNegativeFlag(A);
}

void NESCPU::EOR() {
	Uint16 addr = addressingMode();
	A = A ^ m_emu->m_bus->read(addr);
	testZeroFlag(A);
	testNegativeFlag(A);
}

void NESCPU::ASL() {
	Uint16 addr = 0;
	Uint8 value;
	if (opcode->mode == ACCUMULATOR) {
		value = A;
	} else {
		addr = addressingMode();
		value = m_emu->m_bus->read(addr);
	}
	P = (P & ~0x01) | ((value >> 7) & 0x01);
	value = value << 1;
	if (opcode->mode == ACCUMULATOR) {
		A = value;
	} else {
		m_emu->m_bus->write(addr, value);
	}
	testZeroFlag(value);
	testNegativeFlag(value);
}

void NESCPU::LSR() {
	Uint16 addr = 0;
	Uint8 value;
	if (opcode->mode == ACCUMULATOR) {
		value = A;
	} else {
		addr = addressingMode();
		value = m_emu->m_bus->read(addr);
	}
	P = (P & ~0x01) | (value & 0x01);
	value = value >> 1;
	if (opcode->mode == ACCUMULATOR) {
		A = value;
	} else {
		m_emu->m_bus->write(addr, value);
	}
	testZeroFlag(value);
	testNegativeFlag(value);
}

void NESCPU::ROL() {
	Uint16 addr = 0;
	Uint8 value;
	Uint8 carry_in = (P & 0x01) << 7;
	if (opcode->mode == ACCUMULATOR) {
		value = A;
	} else {
		addr = addressingMode();
		value = m_emu->m_bus->read(addr);
	}
	P = (P & ~0x01) | ((value >> 7) & 0x01);
	value = (value << 1) | carry_in;
	if (opcode->mode == ACCUMULATOR) {
		A = value;
	} else {
		m_emu->m_bus->write(addr, value);
	}
	testZeroFlag(value);
	testNegativeFlag(value);
}

void NESCPU::ROR() {
	Uint16 addr = 0;
	Uint8 value;
	Uint8 carry_in = P & 0x01;
	if (opcode->mode == ACCUMULATOR) {
		value = A;
	} else {
		addr = addressingMode();
		value = m_emu->m_bus->read(addr);
	}
	P = (P & ~0x01) | (value & 0x01);
	value = (value >> 1) | (carry_in << 7);
	if (opcode->mode == ACCUMULATOR) {
		A = value;
	} else {
		m_emu->m_bus->write(addr, value);
	}
	testZeroFlag(value);
	testNegativeFlag(value);
}

void NESCPU::BIT() {
	Uint16 addr = addressingMode();
	Uint8 value = m_emu->m_bus->read(addr);
	Uint8 result = A & value;
	testZeroFlag(result);
	P = (P & 0x3F) | (value & 0xC0);
}

void NESCPU::PHA() {
	pushByte(A);
}

void NESCPU::PLA() {
	A = popByte();
	testZeroFlag(A);
	testNegativeFlag(A);
}

void NESCPU::PHP() {
	pushByte(P | 0x30);
}

void NESCPU::PLP() {
	P = popByte() | 0x20;
}

void NESCPU::NOP() {
}

