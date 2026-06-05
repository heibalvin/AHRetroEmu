import Foundation

enum NESCPUMASK: UInt8 {
    case NEGATIVE           = 0b1000_0000 // Negative
    case OVERFLOW           = 0b0100_0000 // Overflow
    case BREAK              = 0b0001_0000 // Break
    case DECIMAL            = 0b0000_1000 // Decimal
    case INTERRUPT_DISABLE  = 0b0000_0100 // Interrupt Disable
    case ZERO               = 0b0000_0010 // Zero
    case CARRY              = 0b0000_0001 // Carry
}

class NESCPU: NESCOM {
    // 2KB WRAM
    private var WRAM = [UInt8](repeating: 0, count: 0x0800)
    
    // Registers
    private var A: UInt8 = 0x00
    private var X: UInt8 = 0x00
    private var Y: UInt8 = 0x00
    private var SP: UInt8 = 0x00
    private var PC: UInt16 = 0x0000
    private var P: UInt8 = 0x00         // Processor status flags

    override func read(_ addr: UInt16) -> UInt8 {
        return WRAM[Int(addr & 0x0800)]
    }

    override func write(_ addr: UInt16, _ value: UInt8) {
        WRAM[Int(addr & 0x0800)] = value
    }

    // TODO: Implement CPU instructions and step function
    func step() {
        fetch()
        decode()
        execute()
    }

    func fetch() {

    }

    func decode() {

    }

    func execute() {

    }
}