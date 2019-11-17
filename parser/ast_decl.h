#pragma once

#include "ast_stmt.h"

enum AccesibilityName{
    PUBLIC, PRIVATE
};

enum PrimitiveType{
    STRING, INT, FLOAT, BOOL
};

class Decl : public Node{

};




class ClassBodyStmt{
    AccesibilityName access;
    Decl* decl;
};

class ClassBody : public Node{
    std::vector<ClassBodyStmt*> stmts;
};

class ClassDecl: public Decl{
    std::string name;
    ClassBody* body;
};

class FnDecl : public Decl{
    std::string name;
    std::string returnType;
    std::vector<Param*> params;
    StmtBlock* body;

public:
    FnDecl(std::string _returnType,
           std::string _name, std::vector<Param*> _params, StmtBlock* _body) : returnType(_returnType), name(_name), params(_params), body(_body) {}

    void print_node(){
        print_text("FunctionDef:");
        indentLevel++;
        print_text("returnType: " + returnType);
        print_text("name: " + name);
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

class PrimType : public Node{
    PrimitiveType type;
};