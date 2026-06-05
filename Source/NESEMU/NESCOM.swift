import Foundation

class NESCOM {
    var emu: NESEMU

    init(emu: NESEMU) {
        self.emu = emu
    }
    
    func powerOn() {
        // To be implemented
    }
    
    func powerOff() {
        // To be implemented
    }
    
    func reset() {
        // To be implemented
    }

    func read(_ addr: UInt16) -> UInt8 {
        // To be implemented
        return 0x00
    }

    func write(_ addr: UInt16, _ value: UInt8) {
        // To be implemented
    }
}