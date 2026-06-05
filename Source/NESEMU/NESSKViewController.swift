import Cocoa
import SpriteKit

class NESSKViewController: NSViewController {
    override func loadView() {
        let view = SKView()
        view.frame = NSMakeRect(0, 0, 256, 240)
        view.showsFPS = true
        view.showsNodeCount = true
        self.view = view
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // The emu will be set by the AppDelegate
    }
}

