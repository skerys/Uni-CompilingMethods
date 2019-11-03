#pragma once

#include "ast_base.h"
#include <variant>

enum UnaryOp{
    POSITIVE, NEGATIVE, NOT
};

enum ArithOp{
    ADD, SUB, MUL, DIV
};

enum CompareOp{
    E, GE, LE, G, L, NE
};

enum LogicOp{
    AND, OR
};

enum IncDecOp{
    INC, DEC
};

class Expr : Node{};

class AssignExpr : Expr{
    std::unique_ptr<Expr> var;
    std::unique_ptr<Expr> value;
};

class VarExpr : Expr{
    std::string name;
};

class LitExpr : Expr{
    std::variant<std::string, int, float> lit;
};

class ParenExpr : Expr{
    std::unique_ptr<Expr> inside;
};

class UnaryExpr : Expr{
    UnaryOp op;
    std::unique_ptr<Expr> expr;
};

class ArithExpr : Expr{
    ArithOp op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

class CompareExpr : Expr{
    CompareOp op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

class LogicExpr : Expr{
    LogicOp op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};


class ArraySubscriptExpr: Expr{
    std::unique_ptr<Expr> array; //could be a string but how would class.member[1] work then?
    std::unique_ptr<Expr> index;

};

class FnCallExpr : Expr{
    std::unique_ptr<Expr> toCall; //could be a string but how would class.method() work then
    std::vector<std::unique_ptr<Expr>> callParams;
};

class FieldAccessExpr : Expr{
    std::unique_ptr<Expr> expr;
    std::string member;
};

class IncDecExpr : Expr{
    IncDecOp op;
    std::unique_ptr<Expr> expr;
};