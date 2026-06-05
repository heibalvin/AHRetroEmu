
// MARK: - App Delegate
import Cocoa
import SpriteKit

@main
class NESSKAppDelegate: NSObject, NSApplicationDelegate {
    var emu: NESEMU!
    var viewController: NESSKViewController!
    
    func applicationDidFinishLaunching(_ notification: Notification) {
        // Get the ROM file path from command line arguments
        let arguments = ProcessInfo.processInfo.arguments
        guard arguments.count > 1 else {
            print("Usage: nesemu <romfile>")
            NSApplication.shared.terminate(nil)
            return
        }
        let romPath = arguments[1]
        
        do {
            let romData = try Data(contentsOf: URL(fileURLWithPath: romPath))
            let romBytes = [UInt8](romData)
            emu = NESEMU()
            emu.loadROM(romBytes)
        } catch {
            print("Failed to load ROM: \(error)")
            NSApplication.shared.terminate(nil)
            return
        }
        
        // Set up the view
        viewController = NESSKViewController()
        if let skView = viewController.view as? SKView {
            let scene = NESSKScene(size: CGSize(width: 256, height: 240), emu: emu)
            scene.scaleMode = .aspectFit
            skView.presentScene(scene)
        }
        
        let window = NSWindow(contentRect: NSMakeRect(0, 0, 256, 240),
                              styleMask: [.titled, .closable, .miniaturizable, .resizable],
                              backing: .buffered, defer: false)
        window.center()
        window.setFrameAutosaveName("Main Window")
        window.contentView = viewController.view
        window.makeKeyAndOrderFront(nil)
    }
}