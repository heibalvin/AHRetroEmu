import Foundation

class NESBUS: NESCOM {
    override func read(_ addr: UInt16) -> UInt8 {
        if (addr < 0x2000) {
            return emu.cpu.read(addr)
        } else if (addr >= 0x2000 && addr < 0x4000) {
            return emu.ppu.read(addr)
        } else if (addr >= 0x4020) {
            return emu.dsk.readPrgRom(addr)
        }
        return 0x00
    }
    
    override func write(_ addr: UInt16, _ value: UInt8) {
        if (addr < 0x2000) {
            emu.cpu.write(addr, value)
        } else if (addr >= 0x2000 && addr < 0x4000) {
            emu.ppu.write(addr, value)
        } else if (addr >= 0x4020) {
            emu.dsk.writePrgRom(addr, value)
        }
    }
    
    func readWord(_ addr: UInt16) -> UInt16 {
        let low = UInt16(read(addr))
        let high = UInt16(read(addr &+ 1))
        return (high << 8) | low
    }
    
    func writeWord(_ addr: UInt16, _ value: UInt16) {
        write(addr, UInt8(value & 0xFF))
        write(addr &+ 1, UInt8((value >> 8) & 0xFF))
    }
}