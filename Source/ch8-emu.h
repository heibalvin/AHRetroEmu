#ifndef CH8_EMU_H
#define CH8_EMU_H

#include <cstdint>
#include <cstdbool>
#include <cstdio>

class CH8Emu {
public:
    static constexpr uint16_t MEMORY_SIZE = 4096;
    static constexpr uint8_t REGISTERS = 16;
    static constexpr uint8_t STACK_SIZE = 16;
    static constexpr uint8_t KEYS = 16;
    static constexpr uint8_t DISPLAY_WIDTH = 64;
    static constexpr uint8_t DISPLAY_HEIGHT = 32;

    CH8Emu();
    ~CH8Emu();

    void init();
    void reset();
    bool loadROM(const uint8_t* data, uint32_t size);
    void step();
    void setKey(uint8_t key, bool pressed);
    void timersTick();

    uint8_t memory[MEMORY_SIZE];
    uint8_t V[REGISTERS];
    uint16_t I;
    uint16_t PC;
    uint8_t SP;
    uint16_t stack[STACK_SIZE];
    uint8_t display[DISPLAY_HEIGHT][DISPLAY_WIDTH];
    uint8_t keys[KEYS];
    uint8_t DT;
    uint8_t ST;
    bool drawFlag;

private:
    static const uint8_t font[80];

    void op_JP_ADDR(uint16_t addr);
    void op_CALL_ADDR(uint16_t addr);
    void op_SE_VX_BYTE(uint8_t vx, uint8_t byte);
    void op_SNE_VX_BYTE(uint8_t vx, uint8_t byte);
    void op_SE_VX_VY(uint8_t vx, uint8_t vy);
    void op_LD_VX_BYTE(uint8_t vx, uint8_t byte);
    void op_ADD_VX_BYTE(uint8_t vx, uint8_t byte);
    void op_LD_VX_VY(uint8_t vx, uint8_t vy);
    void op_OR_VX_VY(uint8_t vx, uint8_t vy);
    void op_AND_VX_VY(uint8_t vx, uint8_t vy);
    void op_XOR_VX_VY(uint8_t vx, uint8_t vy);
    void op_ADD_VX_VY(uint8_t vx, uint8_t vy);
    void op_SUB_VX_VY(uint8_t vx, uint8_t vy, bool reverse = false);
    void op_SHR_VX(uint8_t vx);
    void op_SHL_VX(uint8_t vx);
    void op_SNE_VX_VY(uint8_t vx, uint8_t vy);
    void op_LD_I_ADDR(uint16_t addr);
    void op_JP_V0_ADDR(uint16_t addr);
    void op_RND_VX_BYTE(uint8_t vx, uint8_t byte);
    void op_DRAW_VX_VY_NIBBLE(uint8_t vx, uint8_t vy, uint8_t n);
    void op_SKP_VX(uint8_t vx);
    void op_SKNP_VX(uint8_t vx);
    void op_LD_VX_DT(uint8_t vx);
    void op_LD_VX_K(uint8_t vx);
    void op_LD_DT_VX(uint8_t vx);
    void op_LD_ST_VX(uint8_t vx);
    void op_ADD_I_VX(uint8_t vx);
    void op_LD_F_VX(uint8_t vx);
    void op_LD_B_VX(uint8_t vx);
    void op_LD_I_VX(uint8_t vx);
    void op_LD_VX_I(uint8_t vx);
};

#endif