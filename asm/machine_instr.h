#pragma once

#include "instructions.h"

class MachineInstruction
{
    uint16_t instruction = 0;

public:
    uint16_t getInstr() const { return instruction; }
    void reset() { instruction = 0; }
    void setOPcode(OPcode code)
    {
        instruction &= 0b1111111111110000;
        instruction |= code;
    }

    void setFunc(uint8_t func)
    {
        instruction &= 0b1111111110001111;
        instruction |= (func & 0b111) << 4;
    }

    void setReg(Register reg, uint8_t reg_index)
    {
        uint8_t offset = (reg_index * 3) + 7;
        uint16_t tmp = ~(0b111 << offset);
        instruction &= tmp;
        instruction |= reg << offset;
    }

    void setR1(Register reg)
    {
        instruction &= 0b1111110001111111;
        instruction |= reg << 7;
    }
    void setR2(Register reg)
    {
        instruction &= 0b1110001111111111;
        instruction |= reg << 10;
    }
    void setR3(Register reg)
    {
        instruction &= 0b0001111111111111;
        instruction |= reg << 13;
    }

    void setImm(uint8_t imm)
    {
        instruction &= 0b0000001111111111;
        instruction |= uint16_t(imm & 0b111111) << 10;
    }

    void setLimm(uint16_t imm)
    {
        instruction &= 0b0000000001111111;
        instruction |= (imm & 0b111111111) << 7;
    }
    void setBit(uint8_t pos)
    {
        instruction |= 1 << pos;
    }
    void resetBit(uint8_t pos)
    {
        instruction &= ~(1 << pos);
    }
    void setBit(uint8_t pos, bool v)
    {
        return v ? setBit(pos) : resetBit(pos);
    }
};