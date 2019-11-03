#pragma once

#include "ast_stmt.h"


class Decl : Node{

};

enum AccesibilityName{
    PUBLIC, PRIVATE
};

class ClassDecl: Decl{
    std::string name;
    std::unique_ptr<ClassBody> body;
};

class ClassBody : Node{
    std::vector<std::unique_ptr<ClassBodyStmt>> stmts;
};

class ClassBodyStmt{
    AccesibilityName access;
    std::unique_ptr<Decl> decl;
};

class FnDecl : Decl{
    std::string name;
    std::string returnType;
    std::vector<std::unique_ptr<Param>> params;
    StmtBlock body;

};

class Program : Node{
    std::vector<std::unique_ptr<Decl>> decls;
}