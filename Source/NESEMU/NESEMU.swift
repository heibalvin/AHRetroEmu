import Foundation

enum NESEMUEvent {
    case none
    case cycle
    case powerup
    case vblank
    case frame
   case nmi
}

class NESEMU {
    var bus: NESBUS!
    var cpu: NESCPU!
    var ppu: NESPPU!
    var dsk: NESDSK!
    var key: NESKEY!
    
    let ntscMasterClock: Double = 21_477_272.0
    var cpuClockInterval: TimeInterval = 0
    var deltaTime: TimeInterval = 0
    var previousTime: TimeInterval = 0
    var timeAccumulator: TimeInterval = 0
    
    var isRunning = false
    var eventAwaiting: NESEMUEvent = .none
    var eventLast: NESEMUEvent = .none
    
    init(mode: NESEMUEvent = .none) {
        bus = NESBUS(emu: self)
        cpu = NESCPU(emu: self)
        ppu = NESPPU(emu: self)
        dsk = NESDSK(emu: self)
        key = NESKEY(emu: self)
        
        cpuClockInterval = 1.0 / (ntscMasterClock / 12)
        
        eventAwaiting = mode
    }
    
    func loadRom(_ data: [UInt8]) {
        dsk.loadRom(data)
    }
    
    func powerOn() {
        ppu.powerOn()
        cpu.powerOn()
        isRunning = true
    }
    
    func powerOff() {
        isRunning = false
    }
    
   func reset() {
   }
    
   func update(_ deltaTime: TimeInterval) {
      // --- STEP 1: EMBARGO EXTRA TIME SLICES IF ALREADY SITTING ON TARGET EVENT ---
      // If an explicit debug milestone is requested and has been hit, we lock
          // execution down. We do NOT run any steps, and we do NOT accumulate time.
          if eventAwaiting != .none && eventLast == eventAwaiting {
              return
          }
                  
          // --- STEP 2: MANUAL STEP MODE ---
          if eventAwaiting == .cycle {
              eventLast = .cycle
              step()
              return
          }
          
          // --- STEP 3: REAL-TIME TIMING INGESTION ---
          timeAccumulator += deltaTime
          let stepsToRun = Int(timeAccumulator / cpuClockInterval)
          timeAccumulator -= Double(stepsToRun) * cpuClockInterval
              
          // --- STEP 4: FLAT ENGINE EXECUTION LOOP ---
          for _ in 0..<stepsToRun {
              step()
              
              // Event Interceptor: Catch the event inside the time slice slice window.
              if eventAwaiting != .none && eventLast == eventAwaiting {
                  // Completely wipe the remainder budget pool so that when we resume
                  // later, we start fresh without an instant multi-frame overflow skip.
                  timeAccumulator = 0
                  break
              }
          }
   }
    
   func step() {
        cpu.step()
        
        ppu.step()
        ppu.step()
        ppu.step()
    }
    
    func setAwaiting(_ event: NESEMUEvent) {
        eventAwaiting = event
        eventLast = .none
    }
    
    func setEvent(_ event: NESEMUEvent) {
        eventLast = event
        print("NESEMU - EVENT: \(event)")
    }
}

