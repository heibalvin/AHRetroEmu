import Foundation

class NESDSK: NESCOM {
    var mapper: Int = 0
    var mirroring: Int = 0

    var prgRomCount = 0
    var prgRomSizeKB = 16
    var prgRoms: [[UInt8]] = []
    var prgRomActive = [ 0, 0]

    var chrRomCount = 0
    var chrRomSizeKB = 8
    var chrRoms: [[UInt8]] = []
    var chrRomActive = [ 0, 0]
    
    func loadROM(_ data: [UInt8]) {
        if data[0] != 0x4E || data[1] != 0x45 || data[2] != 0x53 || data[3] != 0x1A {
            print("Invalid iNES header")
            return
        }
        
        // Parse iNES header
        // Bytes 4: PRG ROM size in units of 16 KB
        prgRomCount = Int(data[4])
        // Byte 5: CHR ROM size in units of 8 KB
        chrRomCount = Int(data[5])
        
        // PRG ROM starts at offset 16
        var addr = 0x0010
        var size = prgRomSizeKB * 1024

        // Load PRG ROM
        for _ in 0..<prgRomCount {
            let prgRom = Array(data[addr..<(addr + size)])
            prgRoms.append(prgRom)
            addr += size
        }
        prgRomActive = [0, prgRomCount - 1]

        // Load CHR ROM
        size = chrRomSizeKB * 1024
        for _ in 0..<chrRomCount {
            let chrRom = Array(data[addr..<(addr + size)])
            chrRoms.append(chrRom)
            addr += size
        }
        chrRomActive = [0, chrRomCount - 1]
    }
    
    // Read from PRG ROM at offset (0x8000-FFFF, but we'll map the entire PRG ROM)
    func readPrgRom(_ addr: UInt16) -> UInt8 {
        if (addr > 0x8000) && (addr < 0xC000) {
            return prgRoms[prgRomActive[0]][Int(addr - 0x8000)]
        } else if (addr >= 0xC000) {
            return prgRoms[prgRomActive[1]][Int(addr - 0xC000)]
        }

        return 0x00
    }
    
    // Write to PRG ROM at offset (0x8000-FFFF, but we'll map the entire PRG ROM)
    func writePrgRom(_ addr: UInt16, _ value: UInt8) {
        if (addr > 0x8000) && (addr < 0xC000) {
            prgRoms[prgRomActive[0]][Int(addr - 0x8000)] = value
        } else if (addr >= 0xC000) {
            prgRoms[prgRomActive[1]][Int(addr - 0xC000)] = value
        }
    }
}