import Foundation

enum NESButton: UInt8 {
    case right = 0
    case left = 1
    case down = 2
    case up = 3
    case start = 4
    case select = 5
    case b = 6
    case a = 7
}

class NESKEY: NESCOM {
    // We'll track the state of each button: true for pressed, false for not pressed
    private var buttonStates: [UInt8] = [UInt8](repeating: 0, count: 8) // 8 buttons
    
    override init(emu: NESEMU) {
        super.init(emu: emu)
    }
    
    func press(_ button: NESButton) {
        buttonStates[Int(button.rawValue)] = 1
    }
    
    func release(_ button: NESButton) {
        buttonStates[Int(button.rawValue)] = 0
    }
    
    func isPressed(_ button: NESButton) -> Bool {
        return buttonStates[Int(button.rawValue)] != 0
    }
    
    // Alternatively, we can return the entire state as a byte (for the $4016/$4017 registers)
    func read() -> UInt8 {
        // The NES controller registers return the button state in the lowest bits, one at a time.
        // But for simplicity, we'll return the entire state as a byte (bit 0: A, bit1: B, etc.)? 
        // Actually, the standard is: 
        //   $4016: bit0 = A (1 pressed), bit1 = B, bit2 = Select, bit3 = Start, bit4 = Up, bit5 = Down, bit6 = Left, bit7 = Right
        // However, the controller is read serially. We'll simplify by returning the entire state in a byte where each bit represents a button.
        // We'll map: 
        //   bit0: A, bit1: B, bit2: Select, bit3: Start, bit4: Up, bit5: Down, bit6: Left, bit7: Right
        var state: UInt8 = 0
        if isPressed(.a) { state |= 0b0000_0001 }
        if isPressed(.b) { state |= 0b0000_0010 }
        if isPressed(.select) { state |= 0b0000_0100 }
        if isPressed(.start) { state |= 0b0000_1000 }
        if isPressed(.up) { state |= 0b0001_0000 }
        if isPressed(.down) { state |= 0b0010_0000 }
        if isPressed(.left) { state |= 0b0100_0000 }
        if isPressed(.right) { state |= 0b1000_0000 }
        return state
    }
}