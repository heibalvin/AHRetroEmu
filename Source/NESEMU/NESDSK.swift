import Foundation

enum NESDSKMirroring: Int {
    case vertical = 0
    case horizontal
}

class NESDSK: NESCOM, CustomStringConvertible {
    var mapper: Int = 0
    var mirroring: NESDSKMirroring = .vertical

    var prgRomCount = 0
    var prgRomSizeKB = 16
    var prgRoms: [[UInt8]] = []
    var prgRomActive = [ 0, 0]

    var chrRomCount = 0
    var chrRomSizeKB = 8
    var chrRoms: [[UInt8]] = []
    var chrRomActive = [ 0, 0]
    
    var description: String {
        return "NESDSK:\n\tMapper: \(mapper)\n\tMirroring: \(mirroring)\n\tPRGROM: \(prgRomCount) * \(prgRomSizeKB) KB\n\tCHRROM: \(chrRomCount) * \(chrRomSizeKB) KB"
    }
    
    func loadRom(_ data: [UInt8]) {
        if data[0] != 0x4E || data[1] != 0x45 || data[2] != 0x53 || data[3] != 0x1A {
            log("NESDSK - ERROR: Invalid iNES header")
            return
        }
        
        mapper = Int((data[7] & 0xF0) | ((data[6] & 0xF0) >> 4))
        mirroring = ((data[6] & 0x01) != 0) ? .vertical : .horizontal
        
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

        if chrRomCount == 0 {
            let chrRamBuffer = [UInt8](repeating: 0, count: 8 * 1024)
            chrRoms.append(chrRamBuffer)
            chrRomActive = [0, 0]
        } else {
            // Load CHR ROM
            size = chrRomSizeKB * 1024
            for _ in 0..<chrRomCount {
                let chrRom = Array(data[addr..<(addr + size)])
                chrRoms.append(chrRom)
                addr += size
            }
            chrRomActive = [0, 0]
        }
    }
    
    // Read from PRG ROM at offset (0x8000-FFFF, but we'll map the entire PRG ROM)
    func readPrgRom(_ addr: UInt16) -> UInt8 {
        let bankIndex = Int(addr & 0x3FFF)
        
        if addr < 0xC000 {
            return prgRoms[prgRomActive[0]][Int(bankIndex)]
        } else {
            return prgRoms[prgRomActive[1]][Int(bankIndex)]
        }
    }
    
    // Write to PRG ROM at offset (0x8000-FFFF, but we'll map the entire PRG ROM)
    func writePrgRom(_ addr: UInt16, _ value: UInt8) {
        if addr < 0xC000 {
            prgRoms[prgRomActive[0]][Int(addr & 0x3FFF)] = value
        } else {
            prgRoms[prgRomActive[1]][Int(addr & 0x3FFF)] = value
        }
    }
    
    func readChrRom(_ addr: UInt16) -> UInt8 {
        let bankIndex = Int(addr & 0x1FFF)
        return chrRoms[chrRomActive[0]][bankIndex]
    }
        
    func writeChrRom(_ addr: UInt16, _ value: UInt8) {
        let bankIndex = Int(addr & 0x1FFF)
        chrRoms[chrRomActive[0]][bankIndex] = value
    }
}
