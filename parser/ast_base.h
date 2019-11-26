#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

static int indentLevel = 0;
bool lastNl = false;
void print_text(std::string text, bool endl){
    //std::cout.clear();
    if(lastNl){
        for(int i = 0; i < indentLevel; i++){
        std::cout << " ";
        }
    }
    std::cout << text << std::flush;
    if(endl){
        std::cout << std::endl;
        lastNl = true;
    }else{
        lastNl = false;
    }
}

void print_text(std::string text){
    print_text(text, true);
}
template<typename ValueType>
std::string stringulate(ValueType v)
{
    std::ostringstream oss;
    oss << v;
    return oss.str();
}


class Node{
    public:
    virtual void print_node(){
        std::cout << "print node not implemented for type " << typeid(this).name() << std::endl;
    }
};

enum PrimitiveTypeName{
    STRING, INT, FLOAT, BOOL, VOID
};

class Type{
public:
    virtual std::string get_type_name() = 0;
};

class PrimitiveType : public Type{
public:
    PrimitiveTypeName type;
    PrimitiveType(PrimitiveTypeName _type) : type(_type) {}
    std::string get_type_name(){
        switch (type)
        {
        case PrimitiveTypeName::BOOL: return "bool";
        case PrimitiveTypeName::INT : return "int";
        case PrimitiveTypeName::FLOAT : return "float";
        case PrimitiveTypeName::STRING : return "string";
        case PrimitiveTypeName::VOID : return "void";
        default : 
            std::cout << "get_type_name error" << std::endl;
            return NULL;
        }
    }
};

class CustomType : public Type{
public:
    Token* customIdent;
    CustomType(Token* ident) : customIdent(ident){}
    std::string get_type_name(){
        return std::get<std::string>(customIdent->value);
    }
};

class Param : public Node{
    Token* name;
    Type* type;
public:
    Param(Type* _type, Token* _name) : type(_type), name(_name) {} 

    void print_node(){
        print_text("Param:");
        indentLevel++;
        print_text("type: " + type->get_type_name());
        print_text("name: " + std::get<std::string>(name->value));

        indentLevel--;
    }
};



