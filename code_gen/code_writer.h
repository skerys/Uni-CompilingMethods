#pragma once
#include "label.h"
#include "instructions.h"
#include <vector>

class CodeWriter{
public:
    std::vector<int> code;
    int i = 1;

    CodeWriter(){}

    void complete_label(Label* label, int value){
        label->value = value;
        for(auto&& offset : label->offsets){
            code[offset] = value;
        }
    }

    void place_label(Label* label){
        complete_label(label, code.size());
    }

    void dump_code()
    {
        //printf("address of codewriter = %d\n", this );
        int offset = 0;
        printf("code size is: %d\n", code.size());
        while(offset < code.size())
        {
            int opcode = code[offset];
            auto instr = instrsByOpcode.find(opcode)->second;
            auto ops = std::vector<int>(code.begin() + offset+1, code.begin()+offset+1+instr.numOps);
            printf("%2d : %-16s", offset, InstrNameStrings[instr.name].c_str());
            for(auto op : ops){
                printf(" %d", op);
            }
            printf("\n");
            offset += 1 + instr.numOps;
        }
        printf("RAW CODE: ");
        for(auto op : code){
            printf(" %d", op);
        }
        printf("\n");
    }


    void write(InstrName instrName, Label* label, std::vector<int> ops){
        
        Instruction instr = instrsByName.find(instrName)->second;
        int opsSize = label != nullptr ? ops.size() + 1 : ops.size();
        if(opsSize != instr.numOps){
            printf("invalid operand count for %s\n", InstrNameStrings[instr.name].c_str());
            exit(0);
        }
        printf("pushing back %d\n", instr.opcode);
        code.push_back(instr.opcode);

        if(label != nullptr){
            if(label->value != -1){
                code.push_back(label->value);
            }else{
                label->offsets.push_back(code.size());
                code.push_back(666);
            }
        }
        
        for(auto&& i : ops){
            printf("pushing back %d", instr.opcode);
            code.push_back(i);
        }
        dump_code();
    }
    
    void write(InstrName instrName, std::vector<int> ops){
        write(instrName, nullptr, ops);
    }

    void write(InstrName instrNme, int op){
        std::vector<int> ops;
        ops.push_back(op);
        write(instrNme, nullptr, ops);
    }

    void write_with_label(InstrName instrName, Label* label){
        write(instrName, label, std::vector<int>());
    }

    void write(InstrName instrName){
        write(instrName, nullptr, std::vector<int>());
    }
};