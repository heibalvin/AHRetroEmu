import Foundation

func log(_ message: @autoclosure () -> String) {
    #if DEBUG
    print(message())
    #endif
}

extension UInt16 {
    func hex() -> String {
        String(format: "%04X", self)
    }
}

extension UInt8 {

    func hex() -> String {
        String(format: "%02X", self)
    }
    
    func bin(_ mask: String) -> String {
        var str = ""
        let chars: [Character] = Array(mask)
        for id in 0...7 {
            if (self & (1 << (7 - id))) != 0 {
                str += "\(chars[id])"
            } else {
                str += "."
            }
        }
        return str
    }
    
	mutating func set(pos: Int, state: Bool) {
        if state {
            self |= 1 << pos
        } else {
            self &= ~(1 << pos)
        }
    }

	mutating func setBit(_ pos: Int) {
        self |= 1 << pos
    }
    
    mutating func clearBit(_ pos: Int) {
        self &= ~(1 << pos)
    }
    
    func getBit(_ pos: Int) -> UInt8 {
        return self & (1 << pos)
    }
    
    func isBitSet(_ pos: Int) -> Bool {
        return (self & (1 << pos)) != 0
    }

	mutating func set(mask: UInt8, state: Bool) {
        if state {
            self |= mask
        } else {
            self &= ~mask
        }
    }

    mutating func setMask(_ mask: UInt8) {
        self |= mask
    }
    
    mutating func clearMask(_ mask: UInt8) {
        self &= ~mask
    }
    
    func getMask(_ mask: UInt8) -> UInt8 {
        return self & mask
    }
    
    func isMaskSet(_ mask: UInt8) -> Bool {
        return (self & mask) != 0
    }
}
