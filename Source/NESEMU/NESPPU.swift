import Foundation

enum NESPPUREG: Int {
    case PPUCTRL     = 0x2000
    case PPUMASK     = 0x2001
    case PPUSTATUS   = 0x2002
    case OAMADDR     = 0x2003
    case OAMDATA     = 0x2004
    case PPUSCROLL   = 0x2005
    case PPUADDR     = 0x2006
    case PPUDATA     = 0x2007
}

class NESPPU: NESCOM {
    // 2KB VRAM
    private var VRAM = [UInt8](repeating: 0, count: 0x0800)
    
    // 256 bytes OAM
    private var OAM = [UInt8](repeating: 0, count: 0x0100)
    
    // 32 bytes PALETTE
    private var PALETTE = [UInt8](repeating: 0, count: 0x20)

    // PPU Registers
    private var REGISTER = [UInt8](repeating: 0, count: 8)

    private var cycle: Int = 0
    private var scanline: Int = 0

    override func read(_ addr: UInt16) -> UInt8 {
        return REGISTER[Int(addr & 0x0007)]
    }

    override func write(_ addr: UInt16, _ value: UInt8) {
        REGISTER[Int(addr & 0x0007)] = value
    }
    
    // TODO: Implement PPU stepping and rendering
    func step() {
        // Increment cycle and handle scanline
        cycle += 1
        if cycle >= 341 {
            cycle = 0
            scanline += 1
            if scanline >= 261 {
                scanline = 0
            }
        }
    }
}