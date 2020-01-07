#pragma once

#include <map>

enum InstrName{
    I_INT_ADD, I_INT_SUB, I_INT_MUL, I_INT_DIV, I_FLOAT_ADD, I_FLOAT_SUB, I_FLOAT_MUL, I_FLOAT_DIV, I_INT_LESS,I_INT_GREATER,I_INT_LESS_EQ,I_INT_GREATER_EQ, I_NOT_EQ, I_EQ, I_GET_L, I_SET_L, I_POP, I_INT_PUSH, I_ALLOC, I_BR, I_BZ, I_CALL_BEGIN, I_CALL, I_RET, I_RET_V,
    I_EXIT
};

std::string InstrNameStrings[] = {"I_INT_ADD", "I_INT_SUB", "I_INT_MUL", "I_INT_DIV", "I_FLOAT_ADD", "I_FLOAT_SUB", "I_FLOAT_MUL", "I_FLOAT_DIV", "I_INT_LESS",
    "I_INT_GREATER","I_INT_LESS_EQ","I_INT_GREATER_EQ", "I_NOT_EQ", "I_EQ", "I_GET_L", "I_SET_L", "I_POP", "I_INT_PUSH", "I_ALLOC", "I_BR", "I_BZ",
    "I_CALL_BEGIN", "I_CALL", "I_RET", "I_RET_V", "I_EXIT"};

class Instruction{
public:
    int opcode;
    InstrName name;
    int numOps;

    Instruction(int _opcode, InstrName _name, int _numOps) : opcode(_opcode), name(_name), numOps(_numOps){}
};

static std::map<InstrName, Instruction> instrsByName;
static std::map<int, Instruction> instrsByOpcode;

void add_instr(int opcode, InstrName name, int numOps){
    Instruction newInstr(opcode, name, numOps);
    instrsByName.insert(std::pair<InstrName, Instruction>(name, newInstr));
    instrsByOpcode.insert(std::pair<int, Instruction>(opcode, newInstr));
}

void setup_instructions(){
    add_instr(0x10, InstrName::I_INT_ADD, 0);
    add_instr(0x11, InstrName::I_INT_SUB, 0);
    add_instr(0x12, InstrName::I_INT_MUL, 0);
    add_instr(0x13, InstrName::I_INT_DIV, 0);
    add_instr(0x14, InstrName::I_FLOAT_ADD, 0);
    add_instr(0x15, InstrName::I_FLOAT_SUB, 0);
    add_instr(0x16, InstrName::I_FLOAT_MUL, 0);
    add_instr(0x17, InstrName::I_FLOAT_DIV, 0);

    add_instr(0x20, InstrName::I_EQ, 0);
    add_instr(0x21, InstrName::I_NOT_EQ, 0);
    add_instr(0x22, InstrName::I_INT_LESS, 0);
    add_instr(0x23, InstrName::I_INT_GREATER, 0);
    add_instr(0x24, InstrName::I_INT_LESS_EQ, 0);
    add_instr(0x25, InstrName::I_INT_GREATER_EQ, 0);

    add_instr(0x30, InstrName::I_GET_L, 1);
    add_instr(0x31, InstrName::I_SET_L, 1);
    add_instr(0x32, InstrName::I_POP, 0);
    add_instr(0x33, InstrName::I_INT_PUSH, 1);
    add_instr(0x34, InstrName::I_ALLOC, 1);

    add_instr(0x40, InstrName::I_BR, 1);
    add_instr(0x41, InstrName::I_BZ, 1);
    add_instr(0x42, InstrName::I_CALL_BEGIN, 0);
    add_instr(0x43, InstrName::I_CALL, 2);
    add_instr(0x44, InstrName::I_RET, 0);
    add_instr(0x45, InstrName::I_RET_V, 0);
    add_instr(0x46, InstrName::I_EXIT, 0);
}

