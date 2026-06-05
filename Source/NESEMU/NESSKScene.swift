// MARK: - SpriteKit Integration
import SpriteKit

class NESSKScene: SKScene {
    let emu: NESEMU
    
    init(size: CGSize, emu: NESEMU) {
        self.emu = emu
        super.init(size: size)
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func didMove(to view: SKView) {
        backgroundColor = .black
    }
    
    override func update(_ currentTime: TimeInterval) {
        // Step the emulator
        emu.step()
        // TODO: Render the PPU output to the scene
    }
}
