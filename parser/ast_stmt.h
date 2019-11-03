#pragma once

#include "ast_expr.h"

class Stmt : Node{

};

class ExprStmr{
    std::unique_ptr<Expr> expr;
};

class IfElseStmt : Stmt{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<StmtBlock> body;
    std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<StmtBlock>>> elifStmts;
    std::unique_ptr<StmtBlock> elseBody;
};

class WhileStmt : Stmt{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<StmtBlock> body;
};

class ForStmt : Stmt{
    std::unique_ptr<Expr> initial;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> final;
    std::unique_ptr<StmtBlock> body;
};

class ReturnStmt : Stmt{
    std::unique_ptr<Expr> value;
};

class BreakStmt : Stmt{};
class NextStmt : Stmt{};

class InputStmt : Stmt{
    std::vector<std::string> idents;
};

class OutputStmt : Stmt{
    std::vector<std::unique_ptr<Expr>> exprs;
};

class DeclareStmt : Stmt{
    std::string type;
    std::string ident;
    std::unique_ptr<Expr> assignExpr;
};

class DeclareStmtArray : Stmt{
    std::string type;
    std::string ident;
    std::unique_ptr<Expr> sizeExpr;
    std::vector<std::unique_ptr<Expr>> assignExprs;
};



class StmtBlock{
    std::vector<std::unique_ptr<Stmt>> statements;
};