#pragma once

#include <unordered_map>
#include <string>

//  rb      ra      rd     func    op code
//|0|0|0| |0|0|0| |0|0|0| |0|0|0| |0|0|0|0| R, M ops
// 15  13 12   10  9   7   6   4   3     0

//     imm        rd     func    op code
//|0|0|0|0|0|0| |0|0|0| |0|0|0| |0|0|0|0|   I ops
// 15        10  9   7   6   4   3     0

//       imm           func    op code
//|0|0|0|0|0|0|0|0|0| |0|0|0| |0|0|0|0|   B ops
// 15              7   6   4   3     0

//       imm           func    op code
//|0|0|0|0|0|0|0|0|0| |0|x|x| |0|0|0|0|   (B1 ops) branch instructions beq, bne, blt, bge 
// 15              7   6   4   3     0
//       imm           func    op code
//|0|0|0|0|0|0|0|0|0| |1|x|x| |0|0|0|0|   (B2 ops) auipc
// 15              7   6   4   3     0
enum OPcode
{
    R = 0b0000,
    I = 0b0001,
    M = 0b0010,
    B = 0b0011,
    INVALID = -1,
};

enum Register
{
    R0 = 0b000, // this register is always ZERO
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,

};

enum Instruction
{
    ADD,
    SUB,
    AND,
    OR,
    XOR,
    SLL,
    SRL,
    SRA,

    ADDI,
    SUBI,
    ANDI,
    ORI,
    XORI,
    SLLI,
    SRLI,
    SRAI,

    LW,
    SW,

    BEQ,
    BNE,
    BLT,
    BGE,
    AUIPC
};

std::unordered_map<std::string, Instruction> instructions{
    {"add", Instruction::ADD},
    {"sub", Instruction::SUB},
    {"and", Instruction::AND},
    {"or", Instruction::OR},
    {"xor", Instruction::XOR},
    {"sll", Instruction::SLL},
    {"srl", Instruction::SRL},
    {"sra", Instruction::SRAI},

    {"addi", Instruction::ADDI},
    {"subi", Instruction::SUBI},
    {"andi", Instruction::ANDI},
    {"ori", Instruction::ORI},
    {"xori", Instruction::XORI},
    {"slli", Instruction::SLLI},
    {"srli", Instruction::SRLI},
    {"srai", Instruction::SRAI},

    {"lw", Instruction::LW},
    {"sw", Instruction::SW},

    {"beq", Instruction::BEQ},
    {"bne", Instruction::BNE},
    {"blt", Instruction::BLT},
    {"bge", Instruction::BGE},
    {"auipc", Instruction::AUIPC},
};

OPcode instructionType(Instruction instr)
{
    if (instr < Instruction::ADD)
        return OPcode::INVALID;
    if (instr <= Instruction::SRA)
        return OPcode::R;
    if (instr <= Instruction::SRAI)
        return OPcode::I;
    if (instr <= Instruction::SW)
        return OPcode::M;
    if (instr <= Instruction::AUIPC)
        return OPcode::B;
    return OPcode::INVALID;
}

uint8_t instructionFunc(Instruction instr)
{
    if (instr <= Instruction::SRA)
        return instr - Instruction::ADD;
    if (instr <= Instruction::SRAI)
        return instr - Instruction::ADDI;
    if (instr <= Instruction::SW)
        return instr - Instruction::LW;
    if (instr <= Instruction::AUIPC)
        return instr - Instruction::BEQ;
    return 0;
}

std::unordered_map<std::string, Register> registers{
    {"r0", Register::R0},
    {"r1", Register::R1},
    {"r2", Register::R2},
    {"r3", Register::R3},
    {"r4", Register::R4},
    {"r5", Register::R5},
    {"r6", Register::R6},
    {"r7", Register::R7},
};