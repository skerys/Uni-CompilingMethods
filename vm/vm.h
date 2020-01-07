#pragma once

#include<vector>
#include <string>
#include "../code_gen/instructions.h"
#include <iostream>
#include "../parser/ast_base.h"

#define STACK_START 1024



class VM{
    std::vector<int> memory = std::vector<int>(4096, 0);
    std::vector<std::string> stringMemory = std::vector<std::string>(50, "");
    bool running = true;
    int ip = 0;
    int fp = STACK_START;
    int sp = STACK_START;

public:
    VM(std::vector<int> code, std::vector<std::string> strings){
        for(auto i = 0; i < code.size(); ++i){
            memory[i] = code[i];
        }

        for(auto i = 0; i < strings.size(); ++i){
            stringMemory[i] = strings[i];
        }
    }

    int pop()
    {
        sp--;
        return memory[sp];
    }
    
    bool push(int value){
        if(sp >= 4096)
        {
            printf("error in %s:%d:%d : StackOverflow\n", fileName.c_str(), 1, 1);
            running = false;
            return false;
        }

        memory[sp] = value;
        sp++;
        return true;
    }

    int read_imm()
    {
        int value = memory[ip];
        ip++;
        return value;
    }

    void exec_call(int target, int numArgs){
        int newIp = target;
        int newFp = sp - numArgs;
        int newSp = newFp;
        memory[newFp-3] = ip;
        memory[newFp-2] = fp;
        memory[newFp-1] = newFp-3;
        ip = newIp;
        fp = newFp;
        sp = newSp;
    }

    void exec_ret(int value){
        int oldIp = memory[fp-3];
        int oldFp = memory[fp-2];
        int oldSp = memory[fp-1];
        ip = oldIp;
        fp = oldFp;
        sp = oldSp;
        if(!push(value)) return;
    }

    void exec()
    {
        while(running){
            exec_one();
        }
        auto result = pop();
        
        printf("result: %d", result);
    }

    void exec_one()
    {
        int hexopcode = read_imm();
        auto opcode = instrsByOpcode.find(hexopcode)->second.name;
        int a, b, i;
        float aa, bb, cc;

        switch(opcode){
            case InstrName::I_INT_ADD:
                b = pop();
                a = pop();
                push(a+b);
                break;
            case InstrName::I_FLOAT_ADD:
                b = pop();
                a = pop();
                aa = reinterpret_cast<float&>(a);
                bb = reinterpret_cast<float&>(b);
                cc = aa+bb;
                push(reinterpret_cast<int&>(cc));
                break;
            case InstrName::I_INT_SUB:
                b = pop();
                a = pop();
                push(a-b);
                break;
            case InstrName::I_FLOAT_SUB:
                b = pop();
                a = pop();
                aa = reinterpret_cast<float&>(a);
                bb = reinterpret_cast<float&>(b);
                cc = aa-bb;
                push(reinterpret_cast<int&>(cc));
                break;
            case InstrName::I_INT_MUL:
                b = pop();
                a = pop();
                push(a*b);
                break;
            case InstrName::I_FLOAT_MUL:
                b = pop();
                a = pop();
                aa = reinterpret_cast<float&>(a);
                bb = reinterpret_cast<float&>(b);
                cc = aa*bb;
                push(reinterpret_cast<int&>(cc));
                break;
            case InstrName::I_INT_DIV:
                b = pop();
                a = pop();
                push(a/b);
                break;
            case InstrName::I_FLOAT_DIV:
                b = pop();
                a = pop();
                aa = reinterpret_cast<float&>(a);
                bb = reinterpret_cast<float&>(b);
                cc = aa/bb;
                push(reinterpret_cast<int&>(cc));
                break;
            case InstrName::I_INT_MOD:
                b = pop();
                a = pop();
                push(a%b);
                break;
            case InstrName::I_UNARY_NEGATIVE:
                a = pop();
                push(-a);
                break;
            case InstrName::I_INT_LESS:
                b = pop();
                a = pop();
                push(a < b ? 1 : 0);
                break;
            case InstrName::I_INT_LESS_EQ:
                b = pop();
                a = pop();
                push(a <= b ? 1 : 0);
                break;
            case InstrName::I_INT_GREATER:
                b = pop();
                a = pop();
                push(a > b ? 1 : 0);
                break;
            case InstrName::I_INT_GREATER_EQ:
                b = pop();
                a = pop();
                push(a >= b ? 1 : 0);
                break;
            case InstrName::I_EQ:
                b = pop();
                a = pop();
                push(a == b ? 1 : 0);
                break;
            case InstrName::I_NOT_EQ:
                b = pop();
                a = pop();
                push(a != b ? 1 : 0);
                break;
            case InstrName::I_GET_L:
                i = read_imm();
                push(memory[fp+i]);
                break;
            case InstrName::I_SET_L:
                i = read_imm();
                memory[fp+i] = pop();
                break;
            case InstrName::I_POP:
                sp--; break;
            case InstrName::I_INT_PUSH:
            case InstrName::I_STR_PUSH:
            case InstrName::I_FLOAT_PUSH:
                push(read_imm());
                break;
            case InstrName::I_ALLOC:
                sp+=read_imm();
                break;
            case InstrName::I_BR:
                i = read_imm();
                ip = i;
                break;
            case InstrName::I_BZ:
                i = read_imm();
                if(pop() == 0){
                    ip = i;
                }
                break;
            case InstrName::I_RET:
                exec_ret(0);
                break;
            case InstrName::I_RET_V:
                exec_ret(pop());
                break;
            case InstrName::I_CALL_BEGIN:
                if(!push(0))return;
                if(!push(0))return;
                if(!push(0))return;
                break;
            case InstrName::I_CALL:
                a = read_imm();
                b = read_imm();
                exec_call(a, b);
                break;
            case InstrName::I_PRINT_INT:
                a = pop();
                printf("%d\n", a);
                break;
            case InstrName::I_PRINT_FLOAT:
                a = pop();
                printf("%f\n", reinterpret_cast<float&>(a));
                break;
            case InstrName::I_PRINT_STRING:
                a = pop();
                printf("%s\n", stringMemory[a].c_str());
                break;
            case InstrName::I_READ_INT:
                printf("read_int:\n");
                a = read_imm();
                b = read_imm();
                int x;
                std::cin >> x;
                push(x);
                if(std::cin.fail()){
                    printf("error in %s:%d:%d : incompatible input error encountered\n", fileName.c_str(), a, b);
                    running = false;
                }
                break;
            case InstrName::I_READ_FLOAT:
                a = read_imm();
                b = read_imm();
                float f;
                std::cin >> f;
                push(reinterpret_cast<int&>(f));
                if(std::cin.fail()){
                    printf("error %d:%d: incompatible input error encountered, exiting..\n", a, b);
                    running = false;
                }
                break;
            case InstrName::I_EXIT:
                running = false;
                break;


        }
    }
};