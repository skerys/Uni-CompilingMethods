#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

class Scope;
class Type;


static int indentLevel = 0;
static int stackSlotIndex = 0;
static Token* lastToken;

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

static std::string fileName;
void print_error(Token* token, std::string error_message){
    printf("error in %s:%d:%d : %s", fileName.c_str(), token->line_no, token->column_no, error_message.c_str());
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
    virtual Token* reference_token(){
         std::cout << "reference token not implemented for type " << typeid(this).name() << std::endl;
         return nullptr;
    }

    virtual Type* get_type(){
        return nullptr;
    }

    virtual Type* check_types()
    {
        std::cout << "check type not implemented for type " << typeid(this).name() << std::endl;
        return nullptr;
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
    STRING, INT, FLOAT, BOOL, VOID, FUNCTION
};

class Type : public Node{
public:
    virtual std::string get_type_name() = 0;

    virtual bool is_comparable(){return false;};
    virtual bool is_arithmethic(){return false;}

    Type* get_type(){
        return this;
    }
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
        case PrimitiveTypeName::FUNCTION : return "function";
        default : 
            std::cout << "get_type_name error" << std::endl;
            return NULL;
        }
    }

    bool is_comparable()
    {
        return type == PrimitiveTypeName::BOOL || type == PrimitiveTypeName::INT || type == PrimitiveTypeName::FLOAT;
    }

    bool is_arithmetic()
    {
        return type == PrimitiveTypeName::INT || type == PrimitiveTypeName::FLOAT;
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


void unify_types(Type* type0, Type* type1, Token* token)
{
    PrimitiveType* prim0;
    PrimitiveType* prim1;
    if(type0 == nullptr || type1 == nullptr){
        return;
    }
    else{
        prim0 = dynamic_cast<PrimitiveType*>(type0);
        prim1 = dynamic_cast<PrimitiveType*>(type1);
    }
    
    if(!(typeid(type0)==typeid(type1))){
     
        print_error(token, "type mismatch");
    }
    else if(prim0 && prim1){
        if(prim0->type != prim1->type){
            //printf("error: type mismatch at %d:%d expected %s, got %s\n", token->column_no, token->line_no, prim0->get_type_name().c_str(), prim1->get_type_name().c_str());

            print_error(token, "type mismatch, expected '" + prim0->get_type_name() + "', got '" + prim1->get_type_name()+"'\n");
        }
    }
    else{
        printf("internal error: unify_types\n");
    }
}

//temporary
void unify_types(Type* type0, Type* type1){
    unify_types(type0, type1, nullptr);
}

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
            print_error(token, "duplicate variable '" + name + "'\n");
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
        print_error(token, "undeclared variable '" + name + "'\n");
        return nullptr;
    }
};

class Param : public Node{
    Token* name;
    int stackSlot;
public:
    Type* type;
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

    Token* reference_token(){
        return name;
    }

    Type* get_type()
    {
        return type;
    }

    Type* check_types()
    {
        auto primitiveType = dynamic_cast<PrimitiveType*>(type);
        if(primitiveType != nullptr){
            if(primitiveType->type == PrimitiveTypeName::VOID){
                //printf("parameter '%s' is of type void at %d:%d", std::get<std::string>(name->value).c_str(), name->line_no, name->column_no);
                print_error(name, "parameter '" + std::get<std::string>(name->value) + "' is of type void\n");
            }
        }
        return nullptr;
    }
};


