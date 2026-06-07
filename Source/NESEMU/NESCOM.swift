import Foundation

class NESCOM {
    var emu: NESEMU

    init(emu: NESEMU) {
        self.emu = emu
    }
    
    func powerOn() {
    }
    
    func powerOff() {
    }
    
    func reset() {
    }

    func read(_ addr: UInt16) -> UInt8 {
        return 0x00
    }

    func write(_ addr: UInt16, _ value: UInt8) {
    }
}
