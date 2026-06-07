//
//  GameScene.swift
//  NESEMUApp Shared
//
//  Created by Alvin HEIB on 05/06/2026.
//

import SpriteKit

class GameScene: SKScene {
    var romname: String = ""
    var emu: NESEMU = NESEMU(mode: .nmi)
    var emuNode = SKNode()
    var ppuNode = SKSpriteNode(texture: SKTexture(), color: .red, size: .zero)
    
    class func newGameScene(romname: String = "Donkey Kong (World) (Rev A)") -> GameScene {
        let scene = GameScene()
        scene.size = CGSize(width: scene.emu.ppu.width, height: scene.emu.ppu.height)
        scene.scaleMode = .aspectFit
        scene.anchorPoint = .zero
        scene.romname = romname
        return scene
    }
    
    override func didMove(to view: SKView) {
        emuNode.name = "emu"
        emuNode.position = CGPoint(x: 0, y: emu.ppu.height)
        emuNode.yScale = -1
        addChild(emuNode)
        
        ppuNode = SKSpriteNode(texture: SKTexture(), color: .red, size: CGSize(width: emu.ppu.width, height: emu.ppu.height))
        ppuNode.name = "ppu"
        ppuNode.blendMode = .replace
        ppuNode.position = .zero
        ppuNode.anchorPoint = .zero
        emuNode.addChild(ppuNode)
        
        if loadRom(romname: romname) {
            log("\(emu.dsk!)")
            emu.powerOn()
        }
    }
    
    override func update(_ currentTime: TimeInterval) {
        emu.update(currentTime)
        
        // Render loop picks up the front buffer frame when the PPU signals it is ready
        if emu.ppu.isDirty {
            screenUpdate()
            emu.ppu.isDirty = false
        }
    }
    
    func screenUpdate() {
        // Safe access via the computed 'buffer' property pointing to the immutable front surface
        let cgImage = emu.ppu.buffer.withUnsafeBytes { unsafeRawBufferPointer -> CGImage? in
            guard let baseAddress = unsafeRawBufferPointer.baseAddress else { return nil }
                        
            guard let provider = CGDataProvider(
                dataInfo: nil,
                data: baseAddress,
                size: emu.ppu.buffer.count,
                releaseData: { _, _, _ in }
            ) else {
                return nil
            }
                        
            let colorSpace = CGColorSpaceCreateDeviceRGB()
            let bitmapInfo = CGBitmapInfo(rawValue: CGImageAlphaInfo.premultipliedLast.rawValue | CGBitmapInfo.byteOrder32Big.rawValue)
                        
            return CGImage(
                width: emu.ppu.width,
                height: emu.ppu.height,
                bitsPerComponent: emu.ppu.bitsPerComponent,
                bitsPerPixel: emu.ppu.bitsPerComponent * emu.ppu.bytesPerPixel,
                bytesPerRow: emu.ppu.bytesPerRow,
                space: colorSpace,
                bitmapInfo: bitmapInfo,
                provider: provider,
                decode: nil,
                shouldInterpolate: false, // Guarantees razor-sharp pixel art rendering
                intent: .defaultIntent
            )
        }
        
        if let cgImage = cgImage {
            let tex = SKTexture(cgImage: cgImage)
            tex.filteringMode = .nearest // Prevents blurring when upscaling the output canvas
            ppuNode.texture = tex
        }
    }
    
    func loadRom(romname: String) -> Bool {
        guard let url = Bundle.main.url(forResource: romname, withExtension: "nes") else {
            log("NESEMUApp: Cannot find rom file: \(romname).nes")
            return false
        }
        
        do {
            let rom = try Data(contentsOf: url)
            emu.loadRom([UInt8](rom))
        } catch {
            log("NESEMUApp: Cannot decode rom file: \(romname).nes")
            return false
        }
        log("NESEMUApp: Loaded rom: \(romname).nes")
        return true
    }
}

#if os(OSX)
extension GameScene {
    override func keyDown(with event: NSEvent) {
        switch event.keyCode {
        case 0x08: emu.setAwaiting(.cycle)   // 'C' key
        case 0x09: emu.setAwaiting(.vblank)  // 'V' key
        case 0x03: emu.setAwaiting(.frame)   // 'F' key
        case 0x23: emu.setAwaiting(.powerup) // 'P' key
        case 0x2D: emu.setAwaiting(.nmi)     // 'N' key
        case 0x31:                           // Spacebar (Toggle Run/Pause)
            if emu.eventAwaiting == .none {
                emu.setAwaiting(.cycle)
            } else {
                emu.setAwaiting(.none)
            }
        default: break
        }
    }
}
#endif
