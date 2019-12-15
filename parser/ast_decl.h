#pragma once

#include "ast_stmt.h"


enum AccesibilityName{
    PUBLIC, PRIVATE
};

enum Label{
    START_LABEL, NO_LABEL
};

class Decl : public Node{
public:
    Token* name;
    Decl(Token* _name) : name(_name){}
};

class Program : public Node{
    std::vector<Decl*> decls;

    public:
    Program(std::vector<Decl*> _decls) : decls(_decls)
    {
        add_children(decls);
    }

    Label mainLabel = Label::NO_LABEL;
    void addDecl(Decl* d){
        decls.push_back(d);
    }

    void print_node(){
        print_text("Program:" );
        indentLevel++;
        for(int i = 0; i < decls.size(); i++){
            print_text("decl["+stringulate(i)+"]:", false);
            decls[i]->print_node();
        }
        indentLevel--;
    }

    void resolve_names(Scope* scope){
        for(auto&& fn : decls){
            scope->add(fn->name, fn);
        }
        for(auto&& fn : decls){
            fn->resolve_names(scope);
        }
        if(mainLabel == Label::NO_LABEL){
            printf("error: no function called main found");
        }
    }
};

//ClassBodyStmt, ClassBody, ClassDecl

class FnDecl : public Decl{

public:
    Type* returnType;
    std::vector<Param*> params;
    StmtBlock* body;
    Scope* mScope;
    int numLocals;
    int stackSlotIndex;
    FnDecl(Type* _returnType,
           Token* _name, std::vector<Param*> _params, StmtBlock* _body) : returnType(_returnType), Decl(_name), params(_params), body(_body) 
    {
        add_children(params);
        add_children(returnType);
        add_children(body);
    }

    void print_node(){
        print_text("FunctionDef:");
        indentLevel++;
        print_text("returnType: " + returnType->get_type_name());
        print_text("name: " + std::get<std::string>(name->value));
        for(int i = 0; i < params.size(); i++){
            print_text("param[" + stringulate(i) +"]:", false);
            params[i]->print_node();
        }
        print_text("body: ", false);
        body->print_node();
        indentLevel--;
    }
    void resolve_names(Scope* scope){
        std::cout << "1" << std::endl;
        if(std::get<std::string>(name->value) == "main"){
            std::cout << "2" << std::endl;
            Program* program = find_ancestor<Program>();
            std::cout << "3" << std::endl;
            program->mainLabel = Label::START_LABEL;
            std::cout << "4" << std::endl;
        }

        stackSlotIndex = 0;
        mScope = new Scope(scope);
        for(auto&& p : params){
            p->resolve_names(mScope);
        }
        body->resolve_names(mScope);
        numLocals = stackSlotIndex;
    }
};


