#pragma once

#include "ast_stmt.h"

enum AccesibilityName{
    PUBLIC, PRIVATE
};

class Decl : public Node{

};

//ClassBodyStmt, ClassBody, ClassDecl

class FnDecl : public Decl{
    Token* name;
    Type* returnType;
    std::vector<Param*> params;
    StmtBlock* body;

public:
    FnDecl(Type* _returnType,
           Token* _name, std::vector<Param*> _params, StmtBlock* _body) : returnType(_returnType), name(_name), params(_params), body(_body) {}

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
};

class Program : public Node{
    std::vector<Decl*> decls;

    public:
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
};
