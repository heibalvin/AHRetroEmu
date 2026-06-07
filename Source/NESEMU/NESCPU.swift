import Foundation

enum NESCPUMASK: UInt8 {
    case NEGATIVE           = 0b1000_0000 // Negative
    case OVERFLOW           = 0b0100_0000 // Overflow
    case UNUSED             = 0b0010_0000 // Unused always 1
    case BREAK              = 0b0001_0000 // Break
    case DECIMAL            = 0b0000_1000 // Decimal
    case INTERRUPT_DISABLE  = 0b0000_0100 // Interrupt Disable
    case ZERO               = 0b0000_0010 // Zero
    case CARRY              = 0b0000_0001 // Carry
}

enum NESCPUMode {
    case implied
    case immediate
    case accumulator
    case zero_page
    case zero_page_indexed_X
    case zero_page_indexed_Y
    case absolute
    case absolute_indexed_X
    case absolute_indexed_Y
    case indirect
    case indexed_indirect
    case indirect_indexed
    case relative
}

struct NESCPUOPC {
    var opc: UInt8
    var mne: String
    var mod: NESCPUMode
    var len: UInt8
    var tim: UInt8
}

class NESCPU: NESCOM {
    var WRAM = [UInt8](repeating: 0, count: 0x0800)     // 2KB WRAM
    
    // Registers
    var A: UInt8 = 0x00
    var X: UInt8 = 0x00
    var Y: UInt8 = 0x00
    var SP: UInt8 = 0x00
    var PC: UInt16 = 0x0000
    var P: UInt8 = 0x00         // Processor status flags

    var isNMIInterrupt: Bool = false
    var isIRQInterrupt: Bool = false
    
    var nextPC: UInt16 = 0x0000
    var waitCycle: UInt8 = 0x00
    var bytes: [UInt8] = [0x00, 0x00, 0x00]
    var opcode: NESCPUOPC? = nil
    var opcodes: [NESCPUOPC] = [
        // --- ADC (Add with Carry) ---
        NESCPUOPC(opc: 0x69, mne: "ADC #$%02X", mod: .immediate, len: 2, tim: 2),
        NESCPUOPC(opc: 0x65, mne: "ADC $%02X", mod: .zero_page, len: 2, tim: 3),
        NESCPUOPC(opc: 0x75, mne: "ADC $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 4),
        NESCPUOPC(opc: 0x6D, mne: "ADC $%04X", mod: .absolute, len: 3, tim: 4),
        NESCPUOPC(opc: 0x7D, mne: "ADC $%04X,X", mod: .absolute_indexed_X, len: 3, tim: 4),
        NESCPUOPC(opc: 0x79, mne: "ADC $%04X,Y", mod: .absolute_indexed_Y, len: 3, tim: 4),
        NESCPUOPC(opc: 0x61, mne: "ADC ($%02X,X)", mod: .indexed_indirect, len: 2, tim: 6),
        NESCPUOPC(opc: 0x71, mne: "ADC ($%02X),Y", mod: .indirect_indexed, len: 2, tim: 5),

        // --- AND (Bitwise AND) ---
        NESCPUOPC(opc: 0x29, mne: "AND #$%02X", mod: .immediate, len: 2, tim: 2),
        NESCPUOPC(opc: 0x25, mne: "AND $%02X", mod: .zero_page, len: 2, tim: 3),
        NESCPUOPC(opc: 0x35, mne: "AND $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 4),
        NESCPUOPC(opc: 0x2D, mne: "AND $%04X", mod: .absolute, len: 3, tim: 4),
        NESCPUOPC(opc: 0x3D, mne: "AND $%04X,X", mod: .absolute_indexed_X, len: 3, tim: 4),
        NESCPUOPC(opc: 0x39, mne: "AND $%04X,Y", mod: .absolute_indexed_Y, len: 3, tim: 4),
        NESCPUOPC(opc: 0x21, mne: "AND ($%02X,X)", mod: .indexed_indirect, len: 2, tim: 6),
        NESCPUOPC(opc: 0x31, mne: "AND ($%02X),Y", mod: .indirect_indexed, len: 2, tim: 5),

        // --- ASL (Arithmetic Shift Left) ---
        NESCPUOPC(opc: 0x0A, mne: "ASL A", mod: .accumulator, len: 1, tim: 2),
        NESCPUOPC(opc: 0x06, mne: "ASL $%02X", mod: .zero_page, len: 2, tim: 5),
        NESCPUOPC(opc: 0x16, mne: "ASL $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 6),
        NESCPUOPC(opc: 0x0E, mne: "ASL $%04X", mod: .absolute, len: 3, tim: 6),
        NESCPUOPC(opc: 0x1E, mne: "ASL $%04X,X", mod: .absolute_indexed_X, len: 3, tim: 7),

        // --- BCC / BCS (Branch on Carry Clear / Set) ---
        NESCPUOPC(opc: 0x90, mne: "BCC $%04X", mod: .relative, len: 2, tim: 2),
        NESCPUOPC(opc: 0xB0, mne: "BCS $%04X", mod: .relative, len: 2, tim: 2),

        // --- BEQ / BNE (Branch on Equal / Not Equal) ---
        NESCPUOPC(opc: 0xF0, mne: "BEQ $%04X", mod: .relative, len: 2, tim: 2),
        NESCPUOPC(opc: 0xD0, mne: "BNE $%04X", mod: .relative, len: 2, tim: 2),

        // --- BMI / BPL (Branch on Minus / Plus) ---
        NESCPUOPC(opc: 0x30, mne: "BMI $%04X", mod: .relative, len: 2, tim: 2),
        NESCPUOPC(opc: 0x10, mne: "BPL $%04X", mod: .relative, len: 2, tim: 2),

        // --- BVC / BVS (Branch on Overflow Clear / Set) ---
        NESCPUOPC(opc: 0x50, mne: "BVC $%04X", mod: .relative, len: 2, tim: 2),
        NESCPUOPC(opc: 0x70, mne: "BVS $%04X", mod: .relative, len: 2, tim: 2),

        // --- BIT (Bit Test) ---
        NESCPUOPC(opc: 0x24, mne: "BIT $%02X", mod: .zero_page, len: 2, tim: 3),
        NESCPUOPC(opc: 0x2C, mne: "BIT $%04X", mod: .absolute, len: 3, tim: 4),

        // --- System Functions (BRK, RTI, RTS) ---
        NESCPUOPC(opc: 0x00, mne: "BRK", mod: .implied, len: 1, tim: 7),
        NESCPUOPC(opc: 0x40, mne: "RTI", mod: .implied, len: 1, tim: 6),
        NESCPUOPC(opc: 0x60, mne: "RTS", mod: .implied, len: 1, tim: 6),

        // --- CLC, CLD, CLI, CLV (Clear Flags) ---
        NESCPUOPC(opc: 0x18, mne: "CLC", mod: .implied, len: 1, tim: 2),
        NESCPUOPC(opc: 0xD8, mne: "CLD", mod: .implied, len: 1, tim: 2),
        NESCPUOPC(opc: 0x58, mne: "CLI", mod: .implied, len: 1, tim: 2),
        NESCPUOPC(opc: 0xB8, mne: "CLV", mod: .implied, len: 1, tim: 2),

        // --- SEC, SED, SEI (Set Flags) ---
        NESCPUOPC(opc: 0x38, mne: "SEC", mod: .implied, len: 1, tim: 2),
        NESCPUOPC(opc: 0xF8, mne: "SED", mod: .implied, len: 1, tim: 2),
        NESCPUOPC(opc: 0x78, mne: "SEI", mod: .implied, len: 1, tim: 2),

        // --- CMP (Compare Accumulator) ---
        NESCPUOPC(opc: 0xC9, mne: "CMP #$%02X", mod: .immediate, len: 2, tim: 2),
        NESCPUOPC(opc: 0xC5, mne: "CMP $%02X", mod: .zero_page, len: 2, tim: 3),
        NESCPUOPC(opc: 0xD5, mne: "CMP $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 4),
        NESCPUOPC(opc: 0xCD, mne: "CMP $%04X", mod: .absolute, len: 3, tim: 4),
        NESCPUOPC(opc: 0xDD, mne: "CMP $%04X,X", mod: .absolute_indexed_X, len: 3, tim: 4),
        NESCPUOPC(opc: 0xD9, mne: "CMP $%04X,Y", mod: .absolute_indexed_Y, len: 3, tim: 4),
        NESCPUOPC(opc: 0xC1, mne: "CMP ($%02X,X)", mod: .indexed_indirect, len: 2, tim: 6),
        NESCPUOPC(opc: 0xD1, mne: "CMP ($%02X),Y", mod: .indirect_indexed, len: 2, tim: 5),

        // --- CPX (Compare X Register) ---
        NESCPUOPC(opc: 0xE0, mne: "CPX #$%02X", mod: .immediate, len: 2, tim: 2),
        NESCPUOPC(opc: 0xE4, mne: "CPX $%02X", mod: .zero_page, len: 2, tim: 3),
        NESCPUOPC(opc: 0xEC, mne: "CPX $%04X", mod: .absolute, len: 3, tim: 4),

        // --- CPY (Compare Y Register) ---
        NESCPUOPC(opc: 0xC0, mne: "CPY #$%02X", mod: .immediate, len: 2, tim: 2),
        NESCPUOPC(opc: 0xC4, mne: "CPY $%02X", mod: .zero_page, len: 2, tim: 3),
        NESCPUOPC(opc: 0xCC, mne: "CPY $%04X", mod: .absolute, len: 3, tim: 4),

        // --- DEC (Decrement Memory) ---
        NESCPUOPC(opc: 0xC6, mne: "DEC $%02X", mod: .zero_page, len: 2, tim: 5),
        NESCPUOPC(opc: 0xD6, mne: "DEC $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 6),
        NESCPUOPC(opc: 0xCE, mne: "DEC $%04X", mod: .absolute, len: 3, tim: 6),
        NESCPUOPC(opc: 0xDE, mne: "DEC $%04X,X", mod: .absolute_indexed_X, len: 3, tim: 7),

        // --- DEX / DEY (Decrement Registers) ---
        NESCPUOPC(opc: 0xCA, mne: "DEX", mod: .implied, len: 1, tim: 2),
        NESCPUOPC(opc: 0x88, mne: "DEY", mod: .implied, len: 1, tim: 2),

        // --- EOR (Bitwise Exclusive OR) ---
        NESCPUOPC(opc: 0x49, mne: "EOR #$%02X", mod: .immediate, len: 2, tim: 2),
        NESCPUOPC(opc: 0x45, mne: "EOR $%02X", mod: .zero_page, len: 2, tim: 3),
        NESCPUOPC(opc: 0x55, mne: "EOR $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 4),
        NESCPUOPC(opc: 0x4D, mne: "EOR $%04X", mod: .absolute, len: 3, tim: 4),
        NESCPUOPC(opc: 0x5D, mne: "EOR $%04X,X", mod: .absolute_indexed_X, len: 3, tim: 4),
        NESCPUOPC(opc: 0x59, mne: "EOR $%04X,Y", mod: .absolute_indexed_Y, len: 3, tim: 4),
        NESCPUOPC(opc: 0x41, mne: "EOR ($%02X,X)", mod: .indexed_indirect, len: 2, tim: 6),
        NESCPUOPC(opc: 0x51, mne: "EOR ($%02X),Y", mod: .indirect_indexed, len: 2, tim: 5),

        // --- INC (Increment Memory) ---
        NESCPUOPC(opc: 0xE6, mne: "INC $%02X", mod: .zero_page, len: 2, tim: 5),
        NESCPUOPC(opc: 0xF6, mne: "INC $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 6),
        NESCPUOPC(opc: 0xEE, mne: "INC $%04X", mod: .absolute, len: 3, tim: 6),
        NESCPUOPC(opc: 0xFE, mne: "INC $%04X,X", mod: .absolute_indexed_X, len: 3, tim: 7),

        // --- INX / INY (Increment Registers) ---
        NESCPUOPC(opc: 0xE8, mne: "INX", mod: .implied, len: 1, tim: 2),
        NESCPUOPC(opc: 0xC8, mne: "INY", mod: .implied, len: 1, tim: 2),

        // --- JMP (Jump) ---
        NESCPUOPC(opc: 0x4C, mne: "JMP $%04X", mod: .absolute, len: 3, tim: 3),
        NESCPUOPC(opc: 0x6C, mne: "JMP ($%04X)", mod: .indirect, len: 3, tim: 5),

        // --- JSR (Jump to Subroutine) ---
        NESCPUOPC(opc: 0x20, mne: "JSR $%04X", mod: .absolute, len: 3, tim: 6),

        // --- LDA (Load Accumulator) ---
        NESCPUOPC(opc: 0xA9, mne: "LDA #$%02X", mod: .immediate, len: 2, tim: 2),
        NESCPUOPC(opc: 0xA5, mne: "LDA $%02X", mod: .zero_page, len: 2, tim: 3),
        NESCPUOPC(opc: 0xB5, mne: "LDA $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 4),
        NESCPUOPC(opc: 0xAD, mne: "LDA $%04X", mod: .absolute, len: 3, tim: 4),
        NESCPUOPC(opc: 0xBD, mne: "LDA $%04X,X", mod: .absolute_indexed_X, len: 3, tim: 4),
        NESCPUOPC(opc: 0xB9, mne: "LDA $%04X,Y", mod: .absolute_indexed_Y, len: 3, tim: 4),
        NESCPUOPC(opc: 0xA1, mne: "LDA ($%02X,X)", mod: .indexed_indirect, len: 2, tim: 6),
        NESCPUOPC(opc: 0xB1, mne: "LDA ($%02X),Y", mod: .indirect_indexed, len: 2, tim: 5),

        // --- LDX (Load X Register) ---
        NESCPUOPC(opc: 0xA2, mne: "LDX #$%02X", mod: .immediate, len: 2, tim: 2),
        NESCPUOPC(opc: 0xA6, mne: "LDX $%02X", mod: .zero_page, len: 2, tim: 3),
        NESCPUOPC(opc: 0xB6, mne: "LDX $%02X,Y", mod: .zero_page_indexed_Y, len: 2, tim: 4),
        NESCPUOPC(opc: 0xAE, mne: "LDX $%04X", mod: .absolute, len: 3, tim: 4),
        NESCPUOPC(opc: 0xBE, mne: "LDX $%04X,Y", mod: .absolute_indexed_Y, len: 3, tim: 4),

        // --- LDY (Load Y Register) ---
        NESCPUOPC(opc: 0xA0, mne: "LDY #$%02X", mod: .immediate, len: 2, tim: 2),
        NESCPUOPC(opc: 0xA4, mne: "LDY $%02X", mod: .zero_page, len: 2, tim: 3),
        NESCPUOPC(opc: 0xB4, mne: "LDY $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 4),
        NESCPUOPC(opc: 0xAC, mne: "LDY $%04X", mod: .absolute, len: 3, tim: 4),
        NESCPUOPC(opc: 0xBC, mne: "LDY $%04X,X", mod: .absolute_indexed_X, len: 3, tim: 4),

        // --- LSR (Logical Shift Right) ---
        NESCPUOPC(opc: 0x4A, mne: "LSR A", mod: .accumulator, len: 1, tim: 2),
        NESCPUOPC(opc: 0x46, mne: "LSR $%02X", mod: .zero_page, len: 2, tim: 5),
        NESCPUOPC(opc: 0x56, mne: "LSR $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 6),
        NESCPUOPC(opc: 0x4E, mne: "LSR $%04X", mod: .absolute, len: 3, tim: 6),
        NESCPUOPC(opc: 0x5E, mne: "LSR $%04X,X", mod: .absolute_indexed_X, len: 3, tim: 7),

        // --- NOP (No Operation) ---
        NESCPUOPC(opc: 0xEA, mne: "NOP", mod: .implied, len: 1, tim: 2),

        // --- ORA (Bitwise OR with Accumulator) ---
        NESCPUOPC(opc: 0x09, mne: "ORA #$%02X", mod: .immediate, len: 2, tim: 2),
        NESCPUOPC(opc: 0x05, mne: "ORA $%02X", mod: .zero_page, len: 2, tim: 3),
        NESCPUOPC(opc: 0x15, mne: "ORA $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 4),
        NESCPUOPC(opc: 0x0D, mne: "ORA $%04X", mod: .absolute, len: 3, tim: 4),
        NESCPUOPC(opc: 0x1D, mne: "ORA $%04X,X", mod: .absolute_indexed_X, len: 3, tim: 4),
        NESCPUOPC(opc: 0x19, mne: "ORA $%04X,Y", mod: .absolute_indexed_Y, len: 3, tim: 4),
        NESCPUOPC(opc: 0x01, mne: "ORA ($%02X,X)", mod: .indexed_indirect, len: 2, tim: 6),
        NESCPUOPC(opc: 0x11, mne: "ORA ($%02X),Y", mod: .indirect_indexed, len: 2, tim: 5),

        // --- Register Stacks (PHA, PHP, PLA, PLP) ---
        NESCPUOPC(opc: 0x48, mne: "PHA", mod: .implied, len: 1, tim: 3),
        NESCPUOPC(opc: 0x08, mne: "PHP", mod: .implied, len: 1, tim: 3),
        NESCPUOPC(opc: 0x68, mne: "PLA", mod: .implied, len: 1, tim: 4),
        NESCPUOPC(opc: 0x28, mne: "PLP", mod: .implied, len: 1, tim: 4),

        // --- ROL (Rotate Left) ---
        NESCPUOPC(opc: 0x2A, mne: "ROL A", mod: .accumulator, len: 1, tim: 2),
        NESCPUOPC(opc: 0x26, mne: "ROL $%02X", mod: .zero_page, len: 2, tim: 5),
        NESCPUOPC(opc: 0x36, mne: "ROL $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 6),
        NESCPUOPC(opc: 0x2E, mne: "ROL $%04X", mod: .absolute, len: 3, tim: 6),
        NESCPUOPC(opc: 0x3E, mne: "ROL $%04X,X", mod: .absolute_indexed_X, len: 3, tim: 7),

        // --- ROR (Rotate Right) ---
        NESCPUOPC(opc: 0x6A, mne: "ROR A", mod: .accumulator, len: 1, tim: 2),
        NESCPUOPC(opc: 0x66, mne: "ROR $%02X", mod: .zero_page, len: 2, tim: 5),
        NESCPUOPC(opc: 0x76, mne: "ROR $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 6),
        NESCPUOPC(opc: 0x6E, mne: "ROR $%04X", mod: .absolute, len: 3, tim: 6),
        NESCPUOPC(opc: 0x7E, mne: "ROR $%04X,X", mod: .absolute_indexed_X, len: 3, tim: 7),

        // --- SBC (Subtract with Carry) ---
        NESCPUOPC(opc: 0xE9, mne: "SBC #$%02X", mod: .immediate, len: 2, tim: 2),
        NESCPUOPC(opc: 0xE5, mne: "SBC $%02X", mod: .zero_page, len: 2, tim: 3),
        NESCPUOPC(opc: 0xF5, mne: "SBC $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 4),
        NESCPUOPC(opc: 0xED, mne: "SBC $%04X", mod: .absolute, len: 3, tim: 4),
        NESCPUOPC(opc: 0xFF, mne: "SBC $%04X,X", mod: .absolute_indexed_X, len: 3, tim: 4),
        NESCPUOPC(opc: 0xF9, mne: "SBC $%04X,Y", mod: .absolute_indexed_Y, len: 3, tim: 4),
        NESCPUOPC(opc: 0xE1, mne: "SBC ($%02X,X)", mod: .indexed_indirect, len: 2, tim: 6),
        NESCPUOPC(opc: 0xF1, mne: "SBC ($%02X),Y", mod: .indirect_indexed, len: 2, tim: 5),

        // --- STA (Store Accumulator) ---
        NESCPUOPC(opc: 0x85, mne: "STA $%02X", mod: .zero_page, len: 2, tim: 3),
        NESCPUOPC(opc: 0x95, mne: "STA $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 4),
        NESCPUOPC(opc: 0x8D, mne: "STA $%04X", mod: .absolute, len: 3, tim: 4),
        NESCPUOPC(opc: 0x9D, mne: "STA $%04X,X", mod: .absolute_indexed_X, len: 3, tim: 4),
        NESCPUOPC(opc: 0x99, mne: "STA $%04X,Y", mod: .absolute_indexed_Y, len: 3, tim: 4),
        NESCPUOPC(opc: 0x81, mne: "STA ($%02X,X)", mod: .indexed_indirect, len: 2, tim: 6),
        NESCPUOPC(opc: 0x91, mne: "STA ($%02X),Y", mod: .indirect_indexed, len: 2, tim: 6),

        // --- STX (Store X Register) ---
        NESCPUOPC(opc: 0x86, mne: "STX $%02X", mod: .zero_page, len: 2, tim: 3),
        NESCPUOPC(opc: 0x96, mne: "STX $%02X,Y", mod: .zero_page_indexed_Y, len: 2, tim: 4),
        NESCPUOPC(opc: 0x8E, mne: "STX $%04X", mod: .absolute, len: 3, tim: 4),

        // --- STY (Store Y Register) ---
        NESCPUOPC(opc: 0x84, mne: "STY $%02X", mod: .zero_page, len: 2, tim: 3),
        NESCPUOPC(opc: 0x94, mne: "STY $%02X,X", mod: .zero_page_indexed_X, len: 2, tim: 4),
        NESCPUOPC(opc: 0x8C, mne: "STY $%04X", mod: .absolute, len: 3, tim: 4),

        // --- Register Transfers (TAX, TAY, TSX, TXA, TXS, TYA) ---
        NESCPUOPC(opc: 0xAA, mne: "TAX", mod: .implied, len: 1, tim: 2),
        NESCPUOPC(opc: 0xA8, mne: "TAY", mod: .implied, len: 1, tim: 2),
        NESCPUOPC(opc: 0xBA, mne: "TSX", mod: .implied, len: 1, tim: 2),
        NESCPUOPC(opc: 0x8A, mne: "TXA", mod: .implied, len: 1, tim: 2),
        NESCPUOPC(opc: 0x9A, mne: "TXS", mod: .implied, len: 1, tim: 2),
        NESCPUOPC(opc: 0x98, mne: "TYA", mod: .implied, len: 1, tim: 2)
    ]
    
    override func powerOn() {
        A = 0x00
        X = 0x00
        Y = 0x00
        SP = 0xFD
        PC = emu.bus.readWord(0xFFFC)
        P = 0x00
        P.setMask(NESCPUMASK.UNUSED.rawValue)
        P.setMask(NESCPUMASK.INTERRUPT_DISABLE.rawValue)
        
        emu.setEvent(.powerup)
    }
    
    override func read(_ addr: UInt16) -> UInt8 {
        return WRAM[Int(addr & 0x07FF)]
    }

    override func write(_ addr: UInt16, _ value: UInt8) {
        WRAM[Int(addr & 0x07FF)] = value
    }

    func step() {
        // 1. Check for Hardware Non-Maskable Interrupts from the PPU before anything else
        if isNMIInterrupt {
            NMI()
            return
        }
            
        if (waitCycle != 0) {
            waitCycle -= 1
            return
        }
            
        fetch()
        decode()
        execute()
    }
        
    private func NMI() {
        emu.setEvent(.nmi)
        
        isNMIInterrupt = false // Latch cleared instantly
        
        // Push current Program Counter High and Low onto the Stack (Page 1)
        emu.bus.write(0x0100 + UInt16(SP), UInt8((PC >> 8) & 0xFF))
        SP = SP &- 1
        emu.bus.write(0x0100 + UInt16(SP), UInt8(PC & 0xFF))
        SP = SP &- 1
        
        // Push Processor status register flags onto stack (Clear Break bit 4, set bit 5)
        var statusPush = P
        // Explicitly format flags for an interrupt stacked context
        statusPush |= NESCPUMASK.UNUSED.rawValue
        statusPush &= ~NESCPUMASK.BREAK.rawValue
        emu.bus.write(0x0100 + UInt16(SP), statusPush)
        SP = SP &- 1
        
        // Lock out interrupt interleaving
        P.setMask(NESCPUMASK.INTERRUPT_DISABLE.rawValue)
        
        // Pull target address location from standard Hardware NMI Vector registers ($FFFA / $FFFB)
        let low = emu.bus.read(0xFFFA)
        let high = emu.bus.read(0xFFFB)
        PC = (UInt16(high) << 8) | UInt16(low)
            
        // A hardware NMI sequence inherently consumes 7 CPU cycles
        waitCycle = 7
    }

    func fetch() {
        bytes[0] = emu.bus.read(PC)
        opcode = opcodes.first(where: { $0.opc == bytes[0] })
        if let opcode = opcode {
            if opcode.len == 2 {
                bytes[1] = emu.bus.read(PC + 1)
            }
            if opcode.len == 3 {
                bytes[1] = emu.bus.read(PC + 1)
                bytes[2] = emu.bus.read(PC + 2)
            }
        } else {
            // TBD: Debug non implemented opcode
            emu.setAwaiting(.cycle)
        }
    }

    func decode() {
        let register = "A: \(A.hex()) X: \(X.hex()) Y: \(Y.hex()) SP: \(SP.hex()) P: \(P.bin("NV1BDIZC"))"
        var instructions = "\(PC.hex()) : \(bytes[0].hex()) .. .."
        var assembler = "Unknown"
        
        if let opcode = opcode {
            if opcode.len == 1 {
                assembler = "\(opcode.mne)"
            } else if opcode.len == 2 {
                instructions = "\(PC.hex()) : \(bytes[0].hex()) \(bytes[1].hex()) .."
                assembler = String(format: opcode.mne, bytes[1])
            } else if opcode.len == 3 {
                instructions = "\(PC.hex()) : \(bytes[0].hex()) \(bytes[1].hex()) \(bytes[2].hex())"
                assembler = String(format: opcode.mne, (UInt16(bytes[2]) << 8) | (UInt16(bytes[1])))
            }
        }
        
        print(register + " | " + instructions + " | " + assembler)
    }

    func execute() {
        if let opcode = opcode {
            nextPC = PC + UInt16(opcode.len)
            
            // Arm the base cycle timing before executing the instruction handler
            waitCycle = opcode.tim
            
            switch opcode.opc {
            // --- ADC ---
            case 0x69, 0x65, 0x75, 0x6D, 0x7D, 0x79, 0x61, 0x71: ADC(); break
                
            // --- AND ---
            case 0x29, 0x25, 0x35, 0x2D, 0x3D, 0x39, 0x21, 0x31: AND(); break
                
            // --- ASL ---
            case 0x0A, 0x06, 0x16, 0x0E, 0x1E:                   ASL(); break
                
            // --- Branches (BCC, BCS, BEQ, BNE, BMI, BPL, BVC, BVS) ---
            case 0x90: BCC(); break
            case 0xB0: BCS(); break
            case 0xF0: BEQ(); break
            case 0xD0: BNE(); break
            case 0x30: BMI(); break
            case 0x10: BPL(); break
            case 0x50: BVC(); break
            case 0x70: BVS(); break
                
            // --- BIT ---
            case 0x24, 0x2C:                                     BIT(); break
                
            // --- System Operations (BRK, RTI, RTS, NOP) ---
            case 0x00: BRK(); break
            case 0x40: RTI(); break
            case 0x60: RTS(); break
            case 0xEA: NOP(); break
                
            // --- Clear Flags (CLC, CLD, CLI, CLV) ---
            case 0x18: CLC(); break
            case 0xD8: CLD(); break
            case 0x58: CLI(); break
            case 0xB8: CLV(); break
                
            // --- Set Flags (SEC, SED, SEI) ---
            case 0x38: SEC(); break
            case 0xF8: SED(); break
            case 0x78: SEI(); break
                
            // --- CMP ---
            case 0xC9, 0xC5, 0xD5, 0xCD, 0xDD, 0xD9, 0xC1, 0xD1: CMP(); break
                
            // --- CPX ---
            case 0xE0, 0xE4, 0xEC:                               CPX(); break
                
            // --- CPY ---
            case 0xC0, 0xC4, 0xCC:                               CPY(); break
                
            // --- DEC ---
            case 0xC6, 0xD6, 0xCE, 0xDE:                         DEC(); break
                
            // --- DEX, DEY ---
            case 0xCA: DEX(); break
            case 0x88: DEY(); break
                
            // --- EOR ---
            case 0x49, 0x45, 0x55, 0x4D, 0x5D, 0x59, 0x41, 0x51: EOR(); break
                
            // --- INC ---
            case 0xE6, 0xF6, 0xEE, 0xFE:                         INC(); break
                
            // --- INX, INY ---
            case 0xE8: INX(); break
            case 0xC8: INY(); break
                
            // --- JMP ---
            case 0x4C, 0x6C:                                     JMP(); break
                
            // --- JSR ---
            case 0x20:                                           JSR(); break
                
            // --- LDA ---
            case 0xA9, 0xA5, 0xB5, 0xAD, 0xBD, 0xB9, 0xA1, 0xB1: LDA(); break
                
            // --- LDX ---
            case 0xA2, 0xA6, 0xB6, 0xAE, 0xBE:                   LDX(); break
                
            // --- LDY ---
            case 0xA0, 0xA4, 0xB4, 0xAC, 0xBC:                   LDY(); break
                
            // --- LSR ---
            case 0x4A, 0x46, 0x56, 0x4E, 0x5E:                   LSR(); break
                
            // --- ORA ---
            case 0x09, 0x05, 0x15, 0x0D, 0x1D, 0x19, 0x01, 0x11: ORA(); break
                
            // --- Stack Operations (PHA, PHP, PLA, PLP) ---
            case 0x48: PHA(); break
            case 0x08: PHP(); break
            case 0x68: PLA(); break
            case 0x28: PLP(); break
                
            // --- ROL ---
            case 0x2A, 0x26, 0x36, 0x2E, 0x3E:                   ROL(); break
                
            // --- ROR ---
            case 0x6A, 0x66, 0x76, 0x6E, 0x7E:                   ROR(); break
                
            // --- SBC ---
            case 0xE9, 0xE5, 0xF5, 0xED, 0xFF, 0xF9, 0xE1, 0xF1: SBC(); break
                
            // --- STA ---
            case 0x85, 0x95, 0x8D, 0x9D, 0x99, 0x81, 0x91:       STA(); break
                
            // --- STX ---
            case 0x86, 0x96, 0x8E:                               STX(); break
                
            // --- STY ---
            case 0x84, 0x94, 0x8C:                               STY(); break
                
            // --- Register Transfers (TAX, TAY, TSX, TXA, TXS, TYA) ---
            case 0xAA: TAX(); break
            case 0xA8: TAY(); break
            case 0xBA: TSX(); break
            case 0x8A: TXA(); break
            case 0x9A: TXS(); break
            case 0x98: TYA(); break
                
            default:
                // Unhandled or unofficial opcode hit: Trigger your debug stop breakpoint trap!
                print(String(format: "NESCPU Error: Unhandled Opcode [0x%02X] at PC: 0x%04X", opcode.opc, PC))
                emu.setAwaiting(.cycle)
                break
            }
        } else {
            nextPC = PC + 1
            waitCycle = 0
        }
        
        PC = nextPC
    }
    
    func getAddrMode() -> UInt16 {
        guard let opcode = opcode else { return PC + 1 }
        
        switch opcode.mod {
        case .implied, .immediate, .accumulator:
            return PC + 1
            
        case .zero_page:
            return UInt16(bytes[1])
            
        case .zero_page_indexed_X:
            return UInt16((bytes[1] &+ X))
            
        case .zero_page_indexed_Y:
            return UInt16((bytes[1] &+ Y))
            
        case .absolute:
            return (UInt16(bytes[2]) << 8) | UInt16(bytes[1])
            
        case .absolute_indexed_X:
            let base = (UInt16(bytes[2]) << 8) | UInt16(bytes[1])
            let target = base &+ UInt16(X)
            // Direct addition: If page changes, add an extra penalty cycle
            if (base & 0xFF00) != (target & 0xFF00) {
                waitCycle += 1
            }
            return target
            
        case .absolute_indexed_Y:
            let base = (UInt16(bytes[2]) << 8) | UInt16(bytes[1])
            let target = base &+ UInt16(Y)
            if (base & 0xFF00) != (target & 0xFF00) {
                waitCycle += 1
            }
            return target
            
        case .relative:
            return PC + 2
            
        case .indirect:
            let lowPointer = (UInt16(bytes[2]) << 8) | UInt16(bytes[1])
            let lowTarget = emu.bus.read(lowPointer)
                
            // If lowPointer is XXFF, high byte is fetched from XX00 due to hardware bug
            let highPointer = (lowPointer & 0x00FF) == 0x00FF ? (lowPointer & 0xFF00) : (lowPointer + 1)
            let highTarget = emu.bus.read(highPointer)
                
            return (UInt16(highTarget) << 8) | UInt16(lowTarget)
            
        case .indexed_indirect:
            let zeroAddress = bytes[1] &+ X
            let lowTarget = emu.bus.read(UInt16(zeroAddress))
            let highTarget = emu.bus.read(UInt16(zeroAddress &+ 1))
            return (UInt16(highTarget) << 8) | UInt16(lowTarget)
            
        case .indirect_indexed:
            let zeroAddress = UInt16(bytes[1])
            let lowBase = emu.bus.read(zeroAddress)
            let highBase = emu.bus.read((zeroAddress + 1) & 0x00FF)
            
            let base = (UInt16(highBase) << 8) | UInt16(lowBase)
            let target = base &+ UInt16(Y)
            if (base & 0xFF00) != (target & 0xFF00) {
                waitCycle += 1
            }
            return target
        }
    }
    
    @inline(__always)
    private func readOperand() -> UInt8 {
        guard let mod = opcode?.mod else { return 0 }
        if mod == .accumulator {
            return A
        }
        let addr = getAddrMode()
        return emu.bus.read(addr)
    }

    @inline(__always)
    private func writeOperand(_ value: UInt8) {
        guard let mod = opcode?.mod else { return }
        if mod == .accumulator {
            A = value
        } else {
            let addr = getAddrMode()
            emu.bus.write(addr, value)
        }
    }

    @inline(__always)
    private func updateNZFlags(_ value: UInt8) {
        P.set(mask: NESCPUMASK.ZERO.rawValue, state: value == 0)
        P.set(mask: NESCPUMASK.NEGATIVE.rawValue, state: (value & 0x80) != 0)
    }
    func LDA() {
        A = readOperand()
        updateNZFlags(A)
    }

    func LDX() {
        X = readOperand()
        updateNZFlags(X)
    }

    func LDY() {
        Y = readOperand()
        updateNZFlags(Y)
    }

    func STA() {
        let addr = getAddrMode()
        emu.bus.write(addr, A)
    }

    func STX() {
        let addr = getAddrMode()
        emu.bus.write(addr, X)
    }

    func STY() {
        let addr = getAddrMode()
        emu.bus.write(addr, Y)
    }

    // MARK: - Register Transfer Operations

    func TAX() { X = A; updateNZFlags(X) }
    func TAY() { Y = A; updateNZFlags(Y) }
    func TSX() { X = SP; updateNZFlags(X) }
    func TXA() { A = X; updateNZFlags(A) }
    func TXS() { SP = X }
    func TYA() { A = Y; updateNZFlags(A) }

    // MARK: - Arithmetic Operations (ADC, SBC)

    func ADC() {
        let value = readOperand()
        let carry: UInt16 = P.isMaskSet(NESCPUMASK.CARRY.rawValue) ? 1 : 0
        let sum = UInt16(A) + UInt16(value) + carry
        
        P.set(mask: NESCPUMASK.CARRY.rawValue, state: sum > 0xFF)
        // Overflow bit: set if sign bit of inputs match, but result sign is different
        let isOverflow = (~(UInt16(A) ^ UInt16(value)) & (UInt16(A) ^ sum) & 0x0080) != 0
        P.set(mask: NESCPUMASK.OVERFLOW.rawValue, state: isOverflow)
        
        A = UInt8(sum & 0xFF)
        updateNZFlags(A)
    }

    func SBC() {
        // SBC on the 6502 is identical to ADC with the bits of the operand inverted
        let value = readOperand() ^ 0xFF
        let carry: UInt16 = P.isMaskSet(NESCPUMASK.CARRY.rawValue) ? 1 : 0
        let sum = UInt16(A) + UInt16(value) + carry
        
        P.set(mask: NESCPUMASK.CARRY.rawValue, state: sum > 0xFF)
        let isOverflow = (~(UInt16(A) ^ UInt16(value)) & (UInt16(A) ^ sum) & 0x0080) != 0
        P.set(mask: NESCPUMASK.OVERFLOW.rawValue, state: isOverflow)
        
        A = UInt8(sum & 0xFF)
        updateNZFlags(A)
    }

    // MARK: - Bitwise Logical Operations

    func AND() { A &= readOperand(); updateNZFlags(A) }
    func EOR() { A ^= readOperand(); updateNZFlags(A) }
    func ORA() { A |= readOperand(); updateNZFlags(A) }

    func BIT() {
        let value = readOperand()
        P.set(mask: NESCPUMASK.ZERO.rawValue, state: (A & value) == 0)
        P.set(mask: NESCPUMASK.NEGATIVE.rawValue, state: (value & 0x80) != 0)
        P.set(mask: NESCPUMASK.OVERFLOW.rawValue, state: (value & 0x40) != 0)
    }

    // MARK: - Shift & Rotate Operations

    func ASL() {
        let value = readOperand()
        P.set(mask: NESCPUMASK.CARRY.rawValue, state: (value & 0x80) != 0)
        let result = value << 1
        writeOperand(result)
        updateNZFlags(result)
    }

    func LSR() {
        let value = readOperand()
        P.set(mask: NESCPUMASK.CARRY.rawValue, state: (value & 0x01) != 0)
        let result = value >> 1
        writeOperand(result)
        updateNZFlags(result)
    }

    func ROL() {
        let value = readOperand()
        let currentCarry: UInt8 = P.isMaskSet(NESCPUMASK.CARRY.rawValue) ? 1 : 0
        P.set(mask: NESCPUMASK.CARRY.rawValue, state: (value & 0x80) != 0)
        let result = (value << 1) | currentCarry
        writeOperand(result)
        updateNZFlags(result)
    }

    func ROR() {
        let value = readOperand()
        let currentCarry: UInt8 = P.isMaskSet(NESCPUMASK.CARRY.rawValue) ? 0x80 : 0
        P.set(mask: NESCPUMASK.CARRY.rawValue, state: (value & 0x01) != 0)
        let result = (value >> 1) | currentCarry
        writeOperand(result)
        updateNZFlags(result)
    }

    // MARK: - Increment & Decrement Operations

    func INC() {
        let result = readOperand() &+ 1
        writeOperand(result)
        updateNZFlags(result)
    }

    func DEC() {
        let result = readOperand() &- 1
        writeOperand(result)
        updateNZFlags(result)
    }

    func INX() { X = X &+ 1; updateNZFlags(X) }
    func INY() { Y = Y &+ 1; updateNZFlags(Y) }
    func DEX() { X = X &- 1; updateNZFlags(X) }
    func DEY() { Y = Y &- 1; updateNZFlags(Y) }

    // MARK: - Comparisons

    func CMP() {
        let value = readOperand()
        P.set(mask: NESCPUMASK.CARRY.rawValue, state: A >= value)
        updateNZFlags(A &- value)
    }

    func CPX() {
        let value = readOperand()
        P.set(mask: NESCPUMASK.CARRY.rawValue, state: X >= value)
        updateNZFlags(X &- value)
    }

    func CPY() {
        let value = readOperand()
        P.set(mask: NESCPUMASK.CARRY.rawValue, state: Y >= value)
        updateNZFlags(Y &- value)
    }

    // MARK: - Branch Operations

    private func branch(condition: Bool) {
        if condition {
            waitCycle += 1 // Penalty cycle for branch success
            
            // bytes[1] contains the signed relative offset (-128 to +127)
            let offset = Int8(bitPattern: bytes[1])
            let basePC = PC + 2
            let targetPC = UInt16(Int(basePC) + Int(offset))
                    
            // Extra penalty cycle if branch crosses a 256-byte page boundary
            if (basePC & 0xFF00) != (targetPC & 0xFF00) {
                waitCycle += 1
            }
            nextPC = targetPC
        }
    }

    func BCC() { branch(condition: !P.isMaskSet(NESCPUMASK.CARRY.rawValue)) }
    func BCS() { branch(condition: P.isMaskSet(NESCPUMASK.CARRY.rawValue)) }
    func BEQ() { branch(condition: P.isMaskSet(NESCPUMASK.ZERO.rawValue)) }
    func BNE() { branch(condition: !P.isMaskSet(NESCPUMASK.ZERO.rawValue)) }
    func BMI() { branch(condition: P.isMaskSet(NESCPUMASK.NEGATIVE.rawValue)) }
    func BPL() { branch(condition: !P.isMaskSet(NESCPUMASK.NEGATIVE.rawValue)) }
    func BVC() { branch(condition: !P.isMaskSet(NESCPUMASK.OVERFLOW.rawValue)) }
    func BVS() { branch(condition: P.isMaskSet(NESCPUMASK.OVERFLOW.rawValue)) }

    // MARK: - Jump & Control Subroutines

    func JMP() {
        nextPC = getAddrMode()
    }

    func JSR() {
        // Push return address minus 1 onto the stack (Little Endian order)
        let returnAddr = PC + 2
        emu.bus.write(0x0100 + UInt16(SP), UInt8((returnAddr >> 8) & 0xFF))
        SP = SP &- 1
        emu.bus.write(0x0100 + UInt16(SP), UInt8(returnAddr & 0xFF))
        SP = SP &- 1
        
        nextPC = (UInt16(bytes[2]) << 8) | UInt16(bytes[1])
    }

    func RTS() {
        SP = SP &+ 1
        let low = emu.bus.read(0x0100 + UInt16(SP))
        SP = SP &+ 1
        let high = emu.bus.read(0x0100 + UInt16(SP))
        
        nextPC = ((UInt16(high) << 8) | UInt16(low)) &+ 1
    }

    // MARK: - Stack Operations

    func PHA() {
        emu.bus.write(0x0100 + UInt16(SP), A)
        SP = SP &- 1
    }

    func PHP() {
        // PHP pushes the processor status byte with the break flags (bits 4 & 5) set to 1
        let statusPush = P | 0x30
        emu.bus.write(0x0100 + UInt16(SP), statusPush)
        SP = SP &- 1
    }

    func PLA() {
        SP = SP &+ 1
        A = emu.bus.read(0x0100 + UInt16(SP))
        updateNZFlags(A)
    }

    func PLP() {
        SP = SP &+ 1
        // Restore status flags, ensuring bit 5 remains 1 and bit 4 (break) is discarded
        P = (emu.bus.read(0x0100 + UInt16(SP)) & 0xEF) | 0x20
    }

    // MARK: - Flag Modification Operations

    func CLC() { P.clearMask(NESCPUMASK.CARRY.rawValue) }
    func CLD() { P.clearMask(NESCPUMASK.DECIMAL.rawValue) }
    func CLI() { P.clearMask(NESCPUMASK.INTERRUPT_DISABLE.rawValue) }
    func CLV() { P.clearMask(NESCPUMASK.OVERFLOW.rawValue) }
    func SEC() { P.setMask(NESCPUMASK.CARRY.rawValue) }
    func SED() { P.setMask(NESCPUMASK.DECIMAL.rawValue) }
    func SEI() { P.setMask(NESCPUMASK.INTERRUPT_DISABLE.rawValue) }

    // MARK: - System Functions

    func NOP() {
        // Do nothing
    }

    func BRK() {
        let returnAddr = PC + 2
        emu.bus.write(0x0100 + UInt16(SP), UInt8((returnAddr >> 8) & 0xFF))
        SP = SP &- 1
        emu.bus.write(0x0100 + UInt16(SP), UInt8(returnAddr & 0xFF))
        SP = SP &- 1
        
        // Push status flag with the BREAK bit manually asserted
        emu.bus.write(0x0100 + UInt16(SP), P | 0x30)
        SP = SP &- 1
        
        P.setMask(NESCPUMASK.INTERRUPT_DISABLE.rawValue)
        
        // Interrupt Vector for BRK/IRQ lives at $FFFE / $FFFF
        let low = emu.bus.read(0xFFFE)
        let high = emu.bus.read(0xFFFF)
        nextPC = (UInt16(high) << 8) | UInt16(low)
    }

    func RTI() {
        SP = SP &+ 1
        P = (emu.bus.read(0x0100 + UInt16(SP)) & 0xEF) | 0x20
        
        SP = SP &+ 1
        let low = emu.bus.read(0x0100 + UInt16(SP))
        SP = SP &+ 1
        let high = emu.bus.read(0x0100 + UInt16(SP))
        
        nextPC = (UInt16(high) << 8) | UInt16(low)
    }
}
