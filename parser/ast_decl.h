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
    std::unique_ptr<Decl> decl;
};

class ClassBody : public Node{
    std::vector<std::unique_ptr<ClassBodyStmt>> stmts;
};

class ClassDecl: public Decl{
    std::string name;
    std::unique_ptr<ClassBody> body;
};

class FnDecl : public Decl{
    std::string name;
    std::string returnType;
    std::vector<std::unique_ptr<Param>> params;
    std::unique_ptr<StmtBlock> body;

public:
    FnDecl(std::string _returnType,
           std::string _name,
           std::vector<std::unique_ptr<Param>> _params,
           std::unique_ptr<StmtBlock> _body) : returnType(_returnType), name(_name), params(_params), body(std::move(_body)) {}
};

class Program : public Node{
    std::vector<std::unique_ptr<Decl>> decls;

    public:
    void addDecl(std::unique_ptr<Decl> d){
        decls.push_back(d);
    }
};

class PrimType : public Node{
    PrimitiveType type;
};