import Foundation

class NESPPU: NESCOM {
    var VRAM = [UInt8](repeating: 0, count: 0x0800)     // 2KB VRAM
    var OAM = [UInt8](repeating: 0, count: 0x0100)      // 256 bytes OAM
    var PALETTE = [UInt8](repeating: 0, count: 0x20)    // 32 bytes PALETTE
    var REGISTER = [UInt8](repeating: 0, count: 8)      // PPU Registers
    let PPUCTRL     = 0
    let PPUMASK     = 1
    let PPUSTATUS   = 2
    let OAMADDR     = 3
    let OAMDATA     = 4
    let PPUSCROLL   = 5
    let PPUADDR     = 6
    let PPUDATA     = 7
    
    var cycle: Int = 0
    var scanline: Int = 0
    
    // --- Renamed Loopy Scroll Registers ---
    var V: UInt16 = 0      // Current VRAM address (15 bits)
    var T: UInt16 = 0      // Temporary VRAM address (15 bits)
    var X: UInt8 = 0       // Fine X scroll (3 bits)
    var W: UInt8 = 0       // First/second write toggle (1 bit)
    
    // Background Latches (temporary holding areas)
    private var bgNextNameTable: UInt8 = 0
    private var bgNextAttribute: UInt8 = 0
    private var bgNextTileLow: UInt8 = 0
    private var bgNextTileHigh: UInt8 = 0
    
    // Background Shift Registers (actively shifting pixel data out)
    private var bgPatternShiftLow: UInt16 = 0
    private var bgPatternShiftHigh: UInt16 = 0
    private var bgAttributeShiftLow: UInt16 = 0
    private var bgAttributeShiftHigh: UInt16 = 0
    
    // Standard NTSC 2C02 PPU System Palette Mapping (RGBA8888)
    private let NTSCPalette: [UInt32] = [
        0x7C7C7CFF, 0x0000FCFF, 0x0000BCFF, 0x4428BCFF, 0x940084FF, 0xA80020FF, 0xA81000FF, 0x881400FF,
        0x503000FF, 0x007800FF, 0x006800FF, 0x005800FF, 0x004058FF, 0x000000FF, 0x000000FF, 0x000000FF,
        0xBCBCBCFF, 0x0078F8FF, 0x0058FAFF, 0x6844FCFF, 0xD800CCFF, 0xE40058FF, 0xF83800FF, 0xE45C10FF,
        0xAC7C00FF, 0x00B800FF, 0x00A800FF, 0x00A844FF, 0x008888FF, 0x000000FF, 0x000000FF, 0x000000FF,
        0xF8F8F8FF, 0x3CBCFCFF, 0x6888FCFF, 0x9878F8FF, 0xF878F8FF, 0xF85898FF, 0xF87858FF, 0xFCA044FF,
        0xF8B800FF, 0xB8F818FF, 0x58D854FF, 0x58F898FF, 0x00E8D8FF, 0x787878FF, 0x000000FF, 0x000000FF,
        0xFCFCFCFF, 0xA4E4FCFF, 0xB8B8F8FF, 0xD8B8F8FF, 0xF8B8F8FF, 0xF8A4C0FF, 0xF0D0B0FF, 0xFCE0A4FF,
        0xFCD878FF, 0xD8F878FF, 0xB8F8B8FF, 0xB8F8D8FF, 0x00FCFCFF, 0xD8D8D8FF, 0x000000FF, 0x000000FF
    ]
    
    let width = 256
    let height = 240
    let bitsPerComponent = 8            // R, G, B and A
    let bitsPerPixel = 8 * 4            // RGBA
    let bytesPerPixel = 4
    let bytesPerRow = 256 * 4
    
    // --- FIXED: Pre-allocate buffers upfront to avoid sizing issues ---
    private var frontBuffer = [UInt8](repeating: 0, count: 256 * 240 * 4)
    private var backBuffer  = [UInt8](repeating: 0, count: 256 * 240 * 4)
    
    // --- FIXED: Public safe accessor for GameScene to consume without naming mismatches ---
    var buffer: [UInt8] { frontBuffer }
    
    // Preserves the delayed VRAM read buffer across different CPU cycles
    private var ppuDataBuffer: UInt8 = 0
    
    var isDirty: Bool = false
    var color: UInt8 = 0
    
    override func powerOn() {
        frontBuffer = [UInt8](repeating: 0, count: width * height * bytesPerPixel)
        backBuffer  = [UInt8](repeating: 0, count: width * height * bytesPerPixel)
        REGISTER = [UInt8](repeating: 0, count: 8)
        scanline = 261
        cycle = 0
        isDirty = false
        
        // Reset scroll registers
        V = 0
        T = 0
        X = 0
        W = 0
    }
    
    override func powerOff() {
        frontBuffer = [UInt8](repeating: 0, count: width * height * bytesPerPixel)
        backBuffer = [UInt8](repeating: 0, count: width * height * bytesPerPixel)
    }
    
    override func read(_ addr: UInt16) -> UInt8 {
        let reg = Int(addr & 0x0007)
        
        switch reg {
        case PPUSTATUS: // Register 2
            let value = REGISTER[PPUSTATUS]
            
            // CRITICAL SIDE EFFECT: Reading PPUSTATUS clears the VBlank flag (bit 7)
            REGISTER[PPUSTATUS] &= ~0x80
            
            // CRITICAL SIDE EFFECT: Reading PPUSTATUS clears the address latch W
            W = 0
            
            return value
            
        case OAMDATA: // Register 4
            return OAM[Int(REGISTER[OAMADDR])]
            
        case PPUDATA: // Register 7
            let currentAddress = V & 0x3FFF
            var value = ppuRead(currentAddress)
            
            if currentAddress < 0x3F00 {
                let bufferedValue = ppuDataBuffer
                ppuDataBuffer = value
                value = bufferedValue
            } else {
                ppuDataBuffer = ppuRead(currentAddress - 0x1000)
            }
            
            let increment: UInt16 = (REGISTER[PPUCTRL] & 0x04) != 0 ? 32 : 1
            V = V &+ increment
            
            return value
            
        default:
            return REGISTER[reg]
        }
    }
    
    override func write(_ addr: UInt16, _ value: UInt8) {
        let reg = Int(addr & 0x0007)
        REGISTER[reg] = value
        
        switch reg {
        case PPUCTRL: // Register 0
            T = (T & 0xF3FF) | (UInt16(value & 0x03) << 10)
            
        case PPUMASK: // Register 1
            break
            
        case OAMADDR: // Register 3
            break
            
        case OAMDATA: // Register 4
            OAM[Int(REGISTER[OAMADDR])] = value
            REGISTER[OAMADDR] = REGISTER[OAMADDR] &+ 1
            
        case PPUSCROLL: // Register 5 (Double Write)
            if W == 0 {
                X = value & 0x07
                T = (T & ~0x001F) | UInt16(value >> 3)
                W = 1
            } else {
                T = (T & ~0x73E0) | (UInt16(value & 0x07) << 12) | (UInt16(value & 0xF8) << 2)
                W = 0
            }
            
        case PPUADDR: // Register 6 (Double Write)
            if W == 0 {
                T = (T & 0x00FF) | (UInt16(value & 0x3F) << 8)
                W = 1
            } else {
                T = (T & 0xFF00) | UInt16(value)
                V = T
                W = 0
            }
            
        case PPUDATA: // Register 7
            let currentAddress = V & 0x3FFF
            ppuWrite(currentAddress, value)
            
            let increment: UInt16 = (REGISTER[PPUCTRL] & 0x04) != 0 ? 32 : 1
            V = (V &+ increment) & 0x3FFF
            
        default:
            break
        }
    }
    
    func ppuRead(_ addr: UInt16) -> UInt8 {
        var maskedAddr = addr & 0x3FFF
        
        if maskedAddr < 0x2000 {
            return emu.dsk.readChrRom(maskedAddr)
        }
        
        // Mirror $3000-$3EFF down to $2000-$2EFF
        if maskedAddr >= 0x3000 && maskedAddr < 0x3F00 {
            maskedAddr -= 0x1000
        }
        
        if maskedAddr >= 0x2000 && maskedAddr < 0x3F00 {
            return VRAM[mapNametableAddress(maskedAddr)]
        }
        
        if maskedAddr >= 0x3F00 {
            var paletteAddr = maskedAddr & 0x001F
            if paletteAddr >= 0x0010 && (paletteAddr & 0x0003) == 0 {
                paletteAddr -= 0x0010
            }
            return PALETTE[Int(paletteAddr)]
        }
        return 0x00
    }
    
    private func ppuWrite(_ addr: UInt16, _ value: UInt8) {
        let address = addr & 0x3FFF
        if address < 0x2000 {
            emu.dsk.writeChrRom(address, value)
        } else if address >= 0x2000 && address < 0x3F00 {
            VRAM[mapNametableAddress(addr)] = value
        } else if addr >= 0x3F00 && addr <= 0x3FFF {
            var paletteAddr = addr & 0x001F
            // Hardware mirror override for transparent color indexes
            if paletteAddr >= 0x0010 && (paletteAddr & 0x0003) == 0 {
                paletteAddr -= 0x0010
            }
            PALETTE[Int(paletteAddr)] = value
        }
    }
    
    func swapBuffers() {
        let temp = frontBuffer
        frontBuffer = backBuffer
        backBuffer = temp
    }
    
    func step() {
        switch scanline {
        case 0...239: // Visible Scanlines
            processVisibleScanline()
        case 240:     // Post-Render
            break     // Idle
        case 241:     // VBlank Entry
            if cycle == 1 {
                // FIXED: Assert proper 0x80 Bit Mask state on PPUSTATUS register
                REGISTER[PPUSTATUS] |= 0x80
                
                // FIXED: Swap working rendering arrays instantly at frame completion
                swapBuffers()
                
                // FIXED: Trigger CPU hardware NMI if NMI generation is enabled in PPUCTRL (Bit 7)
                if (REGISTER[PPUCTRL] & 0x80) != 0 {
                    emu.cpu.isNMIInterrupt = true
                }
                
                emu.setEvent(.vblank)
                isDirty = true
            }
        case 261:     // Pre-Render
            processPreRenderScanline()
            if cycle == 1 {
                // FIXED: Clear Bit 7 mask safely via bitwise operations
                REGISTER[PPUSTATUS] &= 0x7F
                emu.setEvent(.frame)
            }
        default:
            break
        }
        
        // Advance PPU Clock
        cycle += 1
        if cycle > 340 {
            cycle = 0
            scanline += 1
            if scanline > 261   {
                scanline = 0
            }
        }
    }
    
    func processVisibleScanline() {
//        if (cycle == 0) && (scanline == 0) {
//            let id = Int(REGISTER[PPUCTRL & 0x03])
//            let nametable = extractNametable(id)
//            let attributetable = extractAttributeTable(id)
//            let palettetable = extractPaletteTable()
//            log(dumpGrid(data: nametable, label: "NameTable"))
//            log(dumpGrid(data: attributetable, label: "AttributeTable"))
//            log(dumpGrid(data: palettetable, label: "PaletteTable"))
//        }
        
        if cycle >= 1 && cycle <= 256 {
            renderPixel()
            shiftBackgroundRegisters()
        }
        
        if (cycle >= 1 && cycle <= 256) || (cycle >= 321 && cycle <= 336) {
            let fetchPhase = (cycle - 1) % 8
            switch fetchPhase {
            case 1:
                bgNextNameTable = fetchNametableByte()
            case 3:
                bgNextAttribute = fetchAttributeByte()
            case 5:
                bgNextTileLow = fetchTileByte(isHighPlane: false)
            case 7:
                bgNextTileHigh = fetchTileByte(isHighPlane: true)
                loadNextBackgroundTile()
                incrementScrollX()
            default:
                break
            }
        }
        
        if cycle == 256 {
            incrementScrollY()
        }
        
        if cycle == 257 {
            updateHorizontalScrollValues()
        }
        
//        if scanline == 64 && cycle % 8 == 0 {
//            print(String(format: "SL: %3d | CYC: %3d | V: 0x%04X | T: 0x%04X | NT_ID: 0x%02X | TileL: 0x%02X | TileH: 0x%02X | W: %d",
//                         scanline,
//                         cycle,
//                         V,
//                         T,
//                         bgNextNameTable,
//                         bgNextTileLow,
//                         bgNextTileHigh,
//                         W
//                        ))
//        }
    }
    
    private func loadNextBackgroundTile() {
        // Preserve upper byte, insert into lower byte
        bgPatternShiftLow = (bgPatternShiftLow & 0xFF00) | UInt16(bgNextTileLow)
        bgPatternShiftHigh = (bgPatternShiftHigh & 0xFF00) | UInt16(bgNextTileHigh)
        
        // Convert 2-bit attributes into full 8-bit patterns for shifting
        bgAttributeShiftLow = (bgAttributeShiftLow & 0xFF00) | ((bgNextAttribute & 0x01) != 0 ? 0xFF : 0x00)
        bgAttributeShiftHigh = (bgAttributeShiftHigh & 0xFF00) | ((bgNextAttribute & 0x02) != 0 ? 0xFF : 0x00)
    }
    
    private func shiftBackgroundRegisters() {
        bgPatternShiftLow <<= 1
        bgPatternShiftHigh <<= 1
        bgAttributeShiftLow <<= 1
        bgAttributeShiftHigh <<= 1
    }
    
    func processPreRenderScanline() {
        processVisibleScanline()
        
        if cycle >= 280 && cycle <= 304 {
            updateVerticalScrollValues()
        }
    }
    
    private func fetchNametableByte() -> UInt8 {
        let address = 0x2000 | (V & 0x0FFF)
        return ppuRead(address)
    }
    
    private func fetchAttributeByte() -> UInt8 {
        let nametableSelect = V & 0x0C00
        let coarseY = (V >> 5) & 0x001F
        let coarseX = V & 0x001F
        
        let address = 0x23C0 | nametableSelect | ((coarseY / 4) << 3) | (coarseX / 4)
        let attributeByte = ppuRead(address)
        
        let shift = ((coarseY & 2) << 1) | (coarseX & 2)
        return (attributeByte >> shift) & 0x03
    }
    
    private func fetchTileByte(isHighPlane: Bool) -> UInt8 {
        // Bit 4 of PPUCTRL selects the background pattern table base ($0000 or $1000)
        let baseAddress: UInt16 = (REGISTER[PPUCTRL] & 0x10) != 0 ? 0x1000 : 0x0000
        
        // Fine Y scroll represents the exact row offset (0-7) within the 8x8 tile
        let fineY = (V >> 12) & 0x07
        
        // Each tile takes up 16 bytes of data.
        // Shift the tile ID left by 4 (multiply by 16) and isolate the plane offset.
        let tileOffset = UInt16(bgNextNameTable) << 4
        let planeOffset: UInt16 = isHighPlane ? 8 : 0
        
        let address = baseAddress + tileOffset + fineY + planeOffset
        
        return ppuRead(address)
    }
    
    private func incrementScrollX() {
        if (V & 0x001F) == 31 {
            V &= ~0x001F
            V ^= 0x0400
        } else {
            V += 1
        }
    }
    
    private func incrementScrollY() {
        if (V & 0x7000) != 0x7000 {
            V += 0x1000
        } else {
            V &= ~0x7000
            var coarseY = (V & 0x03E0) >> 5
            if coarseY == 29 {
                coarseY = 0
                V ^= 0x0800
            } else if coarseY == 31 {
                coarseY = 0
            } else {
                coarseY += 1
            }
            V = (V & ~0x03E0) | (coarseY << 5)
        }
    }
    
    private func updateHorizontalScrollValues() {
        V = (V & 0xFBE0) | (T & 0x041F)
    }
    
    private func updateVerticalScrollValues() {
        V = (V & 0x041F) | (T & 0x7BE0)
    }
    
    
    private func renderPixel() {
        let x = cycle - 1
        let y = scanline
        
        guard x >= 0 && x < width && y >= 0 && y < height else { return }
        
        let bitMux: UInt16 = 0x8000 >> X
        
        let pixelBit0: UInt8 = (bgPatternShiftLow & bitMux)  != 0 ? 1 : 0
        let pixelBit1: UInt8 = (bgPatternShiftHigh & bitMux) != 0 ? 1 : 0
        let bgPixelColorIndex = (pixelBit1 << 1) | pixelBit0
        
        let paletteBit0: UInt8 = (bgAttributeShiftLow & bitMux)  != 0 ? 1 : 0
        let paletteBit1: UInt8 = (bgAttributeShiftHigh & bitMux) != 0 ? 1 : 0
        let bgPaletteIndex = (paletteBit1 << 1) | paletteBit0
        
        var paletteAddress: UInt16 = 0x3F00
        if bgPixelColorIndex != 0 {
            paletteAddress |= (UInt16(bgPaletteIndex) << 2) | UInt16(bgPixelColorIndex)
        }
        
        let systemColorID = Int(ppuRead(paletteAddress) & 0x3F)
        
        // DEFENSIVE GUARD: Ensure color palette indices do not step past NTSCPalette capacity
        guard systemColorID < NTSCPalette.count else { return }
        let rgbaColor = NTSCPalette[systemColorID]
        
        let bufferOffset = (y * width + x) * bytesPerPixel
        
        // DEFENSIVE GUARD: Avoid out-of-bounds exceptions on the backbuffer array
        guard bufferOffset + 3 < backBuffer.count else { return }
        
        backBuffer[bufferOffset + 0] = UInt8((rgbaColor >> 24) & 0xFF) // R
        backBuffer[bufferOffset + 1] = UInt8((rgbaColor >> 16) & 0xFF) // G
        backBuffer[bufferOffset + 2] = UInt8((rgbaColor >> 8)  & 0xFF) // B
        backBuffer[bufferOffset + 3] = UInt8(rgbaColor & 0xFF)         // A
    }
    
    private func mapNametableAddress(_ addr: UInt16) -> Int {
        let normalizedAddr = addr & 0x0FFF
        switch emu.dsk.mirroring {
        case .vertical:
            return Int(normalizedAddr & 0x07FF)
        case .horizontal:
            // Bit 11 determines which 1KB physical bank to use
            // Clear bit 10 to simulate row-mirroring
            let bank = (normalizedAddr & 0x0800) >> 1
            let offset = normalizedAddr & 0x03FF
            return Int(bank | offset)
        default:
            return Int(normalizedAddr & 0x07FF)
        }
    }
}

extension NESPPU {
    
    /// Extracts a 1024-byte array representing the specified Nametable (0 to 3)
    /// Includes both the 960 bytes of tile IDs and the 64 bytes of attribute data.
    func extractNametable(_ id: Int) -> [UInt8] {
        // NES Nametables start at 0x2000, each taking 0x0400 (1024) bytes
        let ntBase = 0x2000 + (UInt16(id & 0x03) * 0x0400)
        
        // Map the PPU address space to your internal 2KB VRAM array based on your mirroring.
        // Assuming your project uses a helper like `ppuRead(address)` or direct VRAM mapping:
        var data = [UInt8](repeating: 0, count: 1024)
        for i in 0..<960 {
            let address = ntBase + UInt16(i)
            // Replace 'readPPUAddress(address)' with your actual VRAM/Mirroring read function
            data[i] = read(address)
        }
        return data
    }
    
    /// Extracts just the 64-byte Attribute Table for the specified Nametable ID (0 to 3)
    func extractAttributeTable(_ id: Int) -> [UInt8] {
        // Attributes live at the last 64 bytes of each nametable (offset 0x03C0)
        let attrBase = 0x2000 + (UInt16(id & 0x03) * 0x0400) + 0x03C0
        
        var data = [UInt8](repeating: 0, count: 64)
        for i in 0..<64 {
            let address = attrBase + UInt16(i)
            data[i] = read(address)
        }
        return data
    }
    
    /// Extracts the full 32 bytes of Palette RAM (0x3F00 - 0x3F1F)
    func extractPaletteTable() -> [UInt8] {
        var data = [UInt8](repeating: 0, count: 32)
        for i in 0..<32 {
            // Replace with your palette array access or system bus read
            data[i] = PALETTE[i]
        }
        return data
    }
}
