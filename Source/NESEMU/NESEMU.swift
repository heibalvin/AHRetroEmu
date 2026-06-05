import Foundation

class NESEMU {
    var bus: NESBUS!
    var cpu: NESCPU!
    var ppu: NESPPU!
    var dsk: NESDSK!
    var key: NESKEY!
    
    init() {
        bus = NESBUS(emu: self)
        cpu = NESCPU(emu: self)
        ppu = NESPPU(emu: self)
        dsk = NESDSK(emu: self)
        key = NESKEY(emu: self)
    }
    
    func loadROM(_ data: [UInt8]) {
        dsk.loadROM(data)
    }
    
    func step() {
        // Execute one CPU instruction
        cpu.step()
        // PPU steps 3 times per CPU cycle (approximate)
        ppu.step()
        ppu.step()
        ppu.step()
    }
}

