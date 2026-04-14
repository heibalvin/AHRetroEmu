#include "ch8-emu.h"
#include <SDL3/SDL.h>
#include <cstdlib>
#include <cstring>

const uint8_t CH8Emu::font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80
};

CH8Emu::CH8Emu() {
    init();
}

CH8Emu::~CH8Emu() {
}

void CH8Emu::init() {
    reset();
}

void CH8Emu::reset() {
    std::memset(memory, 0, sizeof(memory));
    std::memset(V, 0, sizeof(V));
    std::memset(display, 0, sizeof(display));
    std::memset(keys, 0, sizeof(keys));
    std::memset(stack, 0, sizeof(stack));

    std::memcpy(memory, font, sizeof(font));

    I = 0;
    PC = 0x200;
    SP = 0;
    DT = 0;
    ST = 0;
    drawFlag = true;
    beepFlag = false;
    soundFlag = false;
    isWaitingKey = false;
    waitingKeyTarget = 0;
}

bool CH8Emu::loadROM(const uint8_t* data, uint32_t size) {
    if (!data || size == 0) {
        SDL_Log("ERROR: Invalid rom data");
        return false;
    }

    if (size > MEMORY_SIZE - 0x200) {
        SDL_Log("ROM too large: %u bytes", size);
        return false;
    }

    std::memcpy(&memory[0x200], data, size);
    return true;
}

void CH8Emu::setKey(uint8_t key, bool pressed) {
    if (key < KEYS) {
        keys[key] = pressed ? 1 : 0;
    }
    if (pressed && isWaitingKey && key == waitingKeyTarget) {
        V[waitingKeyTarget] = key;
        isWaitingKey = false;
    }
}

void CH8Emu::timersTick() {
    if (DT > 0) {
        DT--;
        if (DT == 0) beepFlag = true;
    }
    if (ST > 0) {
        ST--;
        if (ST == 0) soundFlag = true;
    }
}

void CH8Emu::op_JP_ADDR(uint16_t addr) {
    PC = addr;
}

void CH8Emu::op_CALL_ADDR(uint16_t addr) {
    if (SP >= STACK_SIZE) return;
    stack[SP++] = PC;
    PC = addr;
}

void CH8Emu::op_SE_VX_BYTE(uint8_t x, uint8_t byte) {
    if (V[x] == byte) PC += 2;
}

void CH8Emu::op_SNE_VX_BYTE(uint8_t x, uint8_t byte) {
    if (V[x] != byte) PC += 2;
}

void CH8Emu::op_SE_VX_VY(uint8_t x, uint8_t y) {
    if (V[x] == V[y]) PC += 2;
}

void CH8Emu::op_LD_VX_BYTE(uint8_t x, uint8_t byte) {
    V[x] = byte;
}

void CH8Emu::op_ADD_VX_BYTE(uint8_t x, uint8_t byte) {
    V[x] += byte;
}

void CH8Emu::op_LD_VX_VY(uint8_t x, uint8_t y) {
    V[x] = V[y];
}

void CH8Emu::op_OR_VX_VY(uint8_t x, uint8_t y) {
    V[x] |= V[y];
    V[0xF] = 0;
}

void CH8Emu::op_AND_VX_VY(uint8_t x, uint8_t y) {
    V[x] &= V[y];
    V[0xF] = 0;
}

void CH8Emu::op_XOR_VX_VY(uint8_t x, uint8_t y) {
    V[x] ^= V[y];
    V[0xF] = 0;
}

void CH8Emu::op_ADD_VX_VY(uint8_t x, uint8_t y) {
    uint16_t sum = V[x] + V[y];
    V[x] = sum & 0xFF;
    V[0xF] = (sum > 255) ? 1 : 0;
}

void CH8Emu::op_SUB_VX_VY(uint8_t x, uint8_t y, bool reverse) {
    uint8_t vx_old = V[x];
    if (reverse) {
        V[x] = V[y] - vx_old;
        V[0xF] = (V[y] >= vx_old) ? 1 : 0;
    } else {
        V[x] = vx_old - V[y];
        V[0xF] = (vx_old >= V[y]) ? 1 : 0;
    }
}

void CH8Emu::op_SHR_VX(uint8_t x, uint8_t y) {
    uint8_t vy_old = V[y];
    V[x] = V[y];
    V[x] = V[x] >> 1;
    V[0xF] = vy_old & 1;
}

void CH8Emu::op_SHL_VX(uint8_t x, uint8_t y) {
    uint8_t vy_old = V[y];
    V[x] = V[y];
    V[x] = V[x] << 1;
    V[0xF] = (vy_old >> 7) & 1;
}

void CH8Emu::op_SNE_VX_VY(uint8_t x, uint8_t y) {
    if (V[x] != V[y]) PC += 2;
}

void CH8Emu::op_LD_I_ADDR(uint16_t addr) {
    I = addr;
}

void CH8Emu::op_JP_V0_ADDR(uint16_t addr) {
    PC = addr + V[0];
}

void CH8Emu::op_RND_VX_BYTE(uint8_t x, uint8_t byte) {
    V[x] = (std::rand() % 256) & byte;
}

void CH8Emu::op_DRAW_VX_VY_NIBBLE(uint8_t x, uint8_t y, uint8_t n) {
    uint8_t px = V[x];
    uint8_t py = V[y];
    
    bool wrapping = (px > 63 || py > 31);
    bool collision = false;

    for (uint8_t i = 0; i < n; i++) {
        uint8_t sprite = memory[I + i];
        for (uint8_t j = 0; j < 8; j++) {
            if ((sprite >> (7 - j)) & 1) {
                uint8_t dx = px + j;
                uint8_t dy = py + i;
                
                if (wrapping) {
                    dx = dx % DISPLAY_WIDTH;
                    dy = dy % DISPLAY_HEIGHT;
                } else {
                    if (dx >= DISPLAY_WIDTH || dy >= DISPLAY_HEIGHT) continue;
                }
                
                if (display[dy][dx]) collision = true;
                display[dy][dx] ^= 1;
            }
        }
    }
    V[0xF] = collision ? 1 : 0;
    drawFlag = true;
}

void CH8Emu::op_SKP_VX(uint8_t x) {
    if (keys[V[x]]) PC += 2;
}

void CH8Emu::op_SKNP_VX(uint8_t x) {
    if (!keys[V[x]]) PC += 2;
}

void CH8Emu::op_LD_VX_DT(uint8_t x) {
    V[x] = DT;
}

void CH8Emu::op_LD_VX_K(uint8_t x) {
    isWaitingKey = true;
    waitingKeyTarget = x;
}

void CH8Emu::op_LD_DT_VX(uint8_t x) {
    DT = V[x];
}

void CH8Emu::op_LD_ST_VX(uint8_t x) {
    ST = V[x];
}

void CH8Emu::op_ADD_I_VX(uint8_t x) {
    I += V[x];
}

void CH8Emu::op_LD_F_VX(uint8_t x) {
    I = V[x] * 5;
}

void CH8Emu::op_LD_B_VX(uint8_t x) {
    uint8_t v = V[x];
    memory[I] = v / 100;
    memory[I + 1] = (v / 10) % 10;
    memory[I + 2] = v % 10;
}

void CH8Emu::op_LD_I_VX(uint8_t x) {
    for (uint8_t i = 0; i <= x; i++) {
        memory[I] = V[i];
        I += 1;
    }
}

void CH8Emu::op_LD_VX_I(uint8_t x) {
    for (uint8_t i = 0; i <= x; i++) {
        V[i] = memory[I];
        I += 1;
    }
}

void CH8Emu::step() {
    if (PC >= MEMORY_SIZE) return;
    if (isWaitingKey) return;

    uint16_t op = (memory[PC] << 8) | memory[PC + 1];
    PC += 2;

    uint8_t nibble = (op >> 12) & 0x0F;
    uint8_t x = (op >> 8) & 0x0F;
    uint8_t y = (op >> 4) & 0x0F;
    uint8_t kk = op & 0xFF;
    uint16_t nnn = op & 0x0FFF;

    switch (nibble) {
        case 0x0:
            if (op == 0x00E0) {
                std::memset(display, 0, sizeof(display));
                drawFlag = true;
            } else if (op == 0x00EE) {
                if (SP > 0) PC = stack[--SP];
            }
            break;
        case 0x1: op_JP_ADDR(nnn); break;
        case 0x2: op_CALL_ADDR(nnn); break;
        case 0x3: op_SE_VX_BYTE(x, kk); break;
        case 0x4: op_SNE_VX_BYTE(x, kk); break;
        case 0x5: op_SE_VX_VY(x, y); break;
        case 0x6: op_LD_VX_BYTE(x, kk); break;
        case 0x7: op_ADD_VX_BYTE(x, kk); break;
        case 0x8:
            switch (op & 0x000F) {
                case 0x0: op_LD_VX_VY(x, y); break;
                case 0x1: op_OR_VX_VY(x, y); break;
                case 0x2: op_AND_VX_VY(x, y); break;
                case 0x3: op_XOR_VX_VY(x, y); break;
                case 0x4: op_ADD_VX_VY(x, y); break;
                case 0x5: op_SUB_VX_VY(x, y, false); break;
                case 0x6: op_SHR_VX(x, y); break;
                case 0x7: op_SUB_VX_VY(x, y, true); break;
                case 0xE: op_SHL_VX(x, y); break;
            }
            break;
        case 0x9: op_SNE_VX_VY(x, y); break;
        case 0xA: I = nnn; break;
        case 0xB: op_JP_V0_ADDR(nnn); break;
        case 0xC: op_RND_VX_BYTE(x, kk); break;
        case 0xD: op_DRAW_VX_VY_NIBBLE(x, y, op & 0x0F); break;
        case 0xE:
            switch (op & 0x00FF) {
                case 0x9E: op_SKP_VX(x); break;
                case 0xA1: op_SKNP_VX(x); break;
            }
            break;
        case 0xF:
            switch (op & 0x00FF) {
                case 0x07: op_LD_VX_DT(x); break;
                case 0x0A: op_LD_VX_K(x); break;
                case 0x15: op_LD_DT_VX(x); break;
                case 0x18: op_LD_ST_VX(x); break;
                case 0x1E: op_ADD_I_VX(x); break;
                case 0x29: op_LD_F_VX(x); break;
                case 0x33: op_LD_B_VX(x); break;
                case 0x55: op_LD_I_VX(x); break;
                case 0x65: op_LD_VX_I(x); break;
            }
            break;
    }
}
