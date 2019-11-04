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

class AssignExpr : public Expr{
    std::unique_ptr<Expr> var;
    std::unique_ptr<Expr> value;
public:
    AssignExpr(std::unique_ptr<Expr> _var, std::unique_ptr<Expr> _value) : var(std::move(_var)), value(std::move(_value)) {}
};

class VarExpr : public Expr{
    std::string name;
public:
    VarExpr(std::string _name) : name(_name){}
};

class LitExpr : public Expr{
    std::variant<std::string, int, float> lit;
public:
    LitExpr(std::variant<std::string, int, float> _lit) : lit(_lit){}
};

class ParenExpr : public Expr{
    std::unique_ptr<Expr> inside;
public:
    ParenExpr(std::unique_ptr<Expr> _inside) : inside(std::move(_inside)) {}
};

class UnaryExpr : public Expr{
    UnaryOp op;
    std::unique_ptr<Expr> expr;
public:
    UnaryExpr(UnaryOp _op, std::unique_ptr<Expr> _expr) : op(_op), expr(std::move(_expr)) {}
};

class ArithExpr : public Expr{
    ArithOp op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
public:
    ArithExpr(ArithOp _op, std::unique_ptr<Expr> _left, std::unique_ptr<Expr> _right) : op(_op), left(std::move(_left)), right(std::move(_right)) {}
};

class CompareExpr : public Expr{
    CompareOp op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
public:
    CompareExpr(CompareOp _op, std::unique_ptr<Expr> _left, std::unique_ptr<Expr> _right) : op(_op), left(std::move(_left)), right(std::move(_right)) {}
};

class LogicExpr : public Expr{
    LogicOp op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
public:
    LogicExpr(LogicOp _op, std::unique_ptr<Expr> _left, std::unique_ptr<Expr> _right) : op(_op), left(std::move(_left)), right(std::move(_right)) {}
};


class ArraySubscriptExpr: public Expr{
    std::unique_ptr<Expr> array; //could be a string but how would class.member[1] work then?
    std::unique_ptr<Expr> index;
public:
    ArraySubscriptExpr(std::unique_ptr<Expr> _array, std::unique_ptr<Expr> _index) : array(std::move(_array)), index(std::move(_index)) {}
};

class FnCallExpr : public Expr{
    std::unique_ptr<Expr> toCall; //could be a string but how would class.method() work then
    std::vector<std::unique_ptr<Expr>> callParams;
public:
    FnCallExpr(std::unique_ptr<Expr> _toCall, std::vector<std::unique_ptr<Expr>> _callParams) : toCall(std::move(_toCall)), callParams(std::move(_callParams)) {}
};

class FieldAccessExpr : public Expr{
    std::unique_ptr<Expr> expr;
    std::string member;
public:
    FieldAccessExpr(std::unique_ptr<Expr> _expr, std::string _member) : expr(std::move(_expr)), member(_member) {}
};

class IncDecExpr : public Expr{
    IncDecOp op;
    std::unique_ptr<Expr> expr;
public:
    IncDecExpr(IncDecOp _op, std::unique_ptr<Expr> _expr) : op(_op), expr(std::move(_expr)) {}
};