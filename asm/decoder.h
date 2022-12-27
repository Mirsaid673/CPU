#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <sstream>
#include <algorithm>
#include "instructions.h"
#include "machine_instr.h"

struct NotResolvedComands
{
    uint32_t index;
    MachineInstruction instruction;
    std::string lable;
};

std::unordered_map<std::string, uint32_t> labels;
std::list<NotResolvedComands> not_resolved_comands;
uint32_t instruction_addr = -1;

enum ReturnCode
{
    SUCCESS = 0,
    INVALID_COMAND,
    INVALID_ARGUMENT_COUNT,
    INVALID_ARGUMENT1,
    INVALID_ARGUMENT2,
    INVALID_ARGUMENT3,
};

bool is_number(const std::string &str);
bool get_reg(const std::string &reg, Register &out);
bool is_word(const std::string &str);

ReturnCode decode(const std::string &cmd, const std::vector<std::string> &args, uint16_t &res)
{
    instruction_addr++;
    auto is_in = instructions.find(cmd);
    if (is_in == instructions.end())
        return ReturnCode::INVALID_COMAND;

    Instruction instruction = is_in->second;

    OPcode op_code = instructionType(instruction);
    MachineInstruction instr;

    instr.setOPcode(op_code);
    if (op_code == OPcode::R || op_code == OPcode::M)
    {
        if (args.size() != 3)
            return ReturnCode::INVALID_ARGUMENT_COUNT;

        instr.setFunc(instructionFunc(instruction));
        for (int i = 0; i < 3; i++)
        {
            Register reg;
            if (!get_reg(args[i], reg))
                return ReturnCode(ReturnCode::INVALID_ARGUMENT1 + i);
            instr.setReg(reg, i);
        }
    }
    else if (op_code == OPcode::I || instruction == Instruction::AUIPC)
    {
        Register reg;
        if (args.size() != 2)
            return ReturnCode::INVALID_ARGUMENT_COUNT;
        if (!get_reg(args[0], reg))
            return ReturnCode::INVALID_ARGUMENT1;
        if (!is_number(args[1]))
            return ReturnCode::INVALID_ARGUMENT2;

        int imm;
        std::stringstream(args[1]) >> imm;

        instr.setFunc(instructionFunc(instruction));
        instr.setR1(reg);
        instr.setImm(imm);
    }
    else if (op_code == OPcode::B)
    {
        if (args.size() != 1)
            return ReturnCode::INVALID_ARGUMENT_COUNT;
        if (is_number(args[0]))
        {
            int imm;
            std::stringstream(args[0]) >> imm;
            instr.setFunc(instructionFunc(instruction));
            instr.setLimm(imm);
        }
        else if (is_word(args[0]))
        {
            instr.setFunc(instructionFunc(instruction));
            const auto is_label = labels.find(args[0]);
            if (is_label == labels.end())
            {
                NotResolvedComands nrc;
                nrc.index = instruction_addr;
                nrc.instruction = instr;
                nrc.lable = args[0];
                not_resolved_comands.push_back(nrc);
            }
            else
            {
                instr.setLimm(is_label->second - instruction_addr);
            }
        }
        else
            return ReturnCode::INVALID_ARGUMENT1;
    }
    res = instr.getInstr();
    return ReturnCode::SUCCESS;
}

bool get_reg(const std::string &reg, Register &out)
{
    auto is_in = registers.find(reg);
    if (is_in == registers.end())
        return false;

    out = is_in->second;
    return true;
}

bool is_number(const std::string &str)
{
    if (str.empty())
        return false;

    if (!(isdigit(str[0]) || str[0] == '-'))
        return false;
    for (int i = 1; i < str.size(); i++)
        if (!isdigit(str[i]))
            return false;

    return true;
}

bool is_word(const std::string &str)
{
    for (int i = 0; i < str.size(); i++)
        if (!isdigit(str[i]) && !isalpha(str[i]) && str[i] != '_')
            return false;
    return true;
}