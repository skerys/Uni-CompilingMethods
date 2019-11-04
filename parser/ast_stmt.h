#pragma once

#include "ast_expr.h"

class Stmt : public Node{
    public:
};

class StmtBlock : public Stmt{
    std::vector<std::unique_ptr<Stmt>> statements;
};

class ExprStmr: public Stmt{
    std::unique_ptr<Expr> expr;
};

class IfElseStmt : public Stmt{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<StmtBlock> body;
    std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<StmtBlock>>> elifStmts;
    std::unique_ptr<StmtBlock> elseBody;
};

class WhileStmt : public Stmt{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<StmtBlock> body;
};

class ForStmt : public Stmt{
    std::unique_ptr<Expr> initial;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> final;
    std::unique_ptr<StmtBlock> body;
};

class ReturnStmt : public Stmt{
    public:
    std::unique_ptr<Expr> value;
public:
    ReturnStmt(std::unique_ptr<Expr> _value) : value(std::move(_value)) {}
};

class BreakStmt : public Stmt{};
class NextStmt : public Stmt{};

class InputStmt : public Stmt{
    std::vector<std::string> idents;
};

class OutputStmt : public Stmt{
    std::vector<std::unique_ptr<Expr>> exprs;
};

class DeclareStmt : public Stmt{
    std::string type;
    std::string ident;
    std::unique_ptr<Expr> assignExpr;
};

class DeclareStmtArray : public Stmt{
    std::string type;
    std::string ident;
    std::unique_ptr<Expr> sizeExpr;
    std::vector<std::unique_ptr<Expr>> assignExprs;
};


