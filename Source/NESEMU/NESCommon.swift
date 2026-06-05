import Foundation

extension UInt8 {

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