#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

class Scope;


static int indentLevel = 0;
static int stackSlotIndex = 0;

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
    Node* parent = nullptr;
    virtual void print_node(){
        std::cout << "print node not implemented for type " << typeid(this).name() << std::endl;
    }
    virtual void resolve_names(Scope* scope){
        std::cout << "resolve name not implemented for type " << typeid(this).name() << std::endl;
    }

    template<typename T, typename Enable=std::enable_if<std::is_base_of<T, Node>::value>>
    void add_children(std::vector<T*> children){
        for(auto v : children){
            if(v){
                v->parent = this;
            }
        }
    }
    template<typename T, typename Enable=std::enable_if<std::is_base_of<T, Node>::value>>
    void add_children(T* child){
        if(child){
            child->parent = this;
        }
    }


    template <typename T>
    T* find_ancestor(){
        Node* current = parent;
        while(current){
            if(dynamic_cast<T*>(current)){
                return dynamic_cast<T*>(current);
            }
            else{
                current = current->parent;
            }
        }
        return nullptr;
    }
};

enum PrimitiveTypeName{
    STRING, INT, FLOAT, BOOL, VOID
};

class Type : public Node{
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


class Scope{
public:
    Scope* parentScope;
    std::map<std::string, Node*> members;
    Scope(Scope* _parentScope) : parentScope(_parentScope) {} 

    void add(Token* token, Node* node){
        auto name = std::get<std::string>(token->value);
        if(members.find(name) == members.end()){
            //not found
            members[name] = node;
        }else{
            //found
            printf("duplicate variable: %s at %d:%d", name, token->line_no, token->column_no);
        }
    }

    Node* resolve_name(Token* token){
        auto name = std::get<std::string>(token->value);
        if(members.find(name) != members.end()){
            //found
            return members[name];
        }

        if(parentScope != nullptr){
            return parentScope->resolve_name(token);
        }

        printf("undeclared variable: %s at %d:%d", name, token->line_no, token->column_no);
    }
};

class Param : public Node{
    Token* name;
    Type* type;
    int stackSlot;
public:
    Param(Type* _type, Token* _name) : type(_type), name(_name) {
        add_children(type);
    } 

    void print_node(){
        print_text("Param:");
        indentLevel++;
        print_text("type: " + type->get_type_name());
        print_text("name: " + std::get<std::string>(name->value));

        indentLevel--;
    }

    void resolve_names(Scope* scope){
        scope->add(name, this);
        stackSlot = stackSlotIndex;
        stackSlotIndex++;
    }
};


