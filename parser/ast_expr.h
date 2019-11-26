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

class Expr : public Node{};

class VarExpr : public Expr{
    Token* var;
public:
    VarExpr(Token* _var) : var(_var){}
    void print_node(){
        print_text("VarExpr:");
        indentLevel++;
        print_text("name: " + std::get<std::string>(var->value));
        indentLevel--;
    }
};

class LitExpr : public Expr{
    std::variant<std::string, int, float> lit;
public:
    LitExpr(std::variant<std::string, int, float> _lit) : lit(_lit){}
void print_node(){
        print_text("LitExpr: ");
        indentLevel++;
        print_text("lit: ", false);
        std::visit(TokenValueVisitor{}, lit);
        std::cout << std::endl;
        lastNl = true;
        indentLevel--;
    }
};

class ParenExpr : public Expr{
    Expr* inside;
public:
    ParenExpr(Expr* _inside) : inside(_inside) {}
    void print_node(){
        print_text("ParenExpr:");
        indentLevel++;
        print_text("inside: ", false);
        inside->print_node();
        indentLevel--;
    }
};

class UnaryExpr : public Expr{
    UnaryOp op;
    Expr* expr;
public:
    UnaryExpr(UnaryOp _op, Expr* _expr) : op(_op), expr(_expr) {}
    void print_node(){
        std::string subType;
        switch(op){
            case UnaryOp::NEGATIVE : subType = "NEGATIVE"; break;
            case UnaryOp::POSITIVE : subType = "POSITIVE"; break;
            case UnaryOp::NOT : subType = "NOT"; break;
        }
        print_text("UnaryExpr("+ subType +"):");
        indentLevel++;
        print_text("expr: ", false);
        expr->print_node();
        indentLevel--;
    }
};

class ArithExpr : public Expr{
    ArithOp op;
    Expr* left;
    Expr* right;
public:
    ArithExpr(ArithOp _op, Expr* _left, Expr* _right) : op(_op), left(_left), right(_right) {}
    void print_node(){
        std::string subType;
        switch(op){
            case ArithOp::ADD : subType = "ADD"; break;
            case ArithOp::SUB : subType = "SUB"; break;
            case ArithOp::MUL : subType = "MUL"; break;
            case ArithOp::DIV : subType = "DIV"; break;
        }
        print_text("ArithExpr("+ subType +"):");
        indentLevel++;
        print_text("left: ", false);
        left->print_node();
        print_text("right: ", false);
        right->print_node();
        indentLevel--;
    }
};

class CompareExpr : public Expr{
    CompareOp op;
    Expr* left;
    Expr* right;
public:
    CompareExpr(CompareOp _op, Expr* _left, Expr* _right) : op(_op), left(_left), right(_right) {}
    void print_node(){
        std::string subType;
        switch(op){
            case CompareOp::E : subType = "EQUAL"; break;
            case CompareOp::L : subType = "LOWER"; break;
            case CompareOp::G : subType = "GREATER"; break;
            case CompareOp::GE : subType = "GREATER_OR_E"; break;
            case CompareOp::LE : subType = "LOWER_OR_E"; break;
        }
        print_text("CompareExpr("+ subType +"):");
        indentLevel++;
        print_text("left: ", false);
        left->print_node();
        print_text("right: ", false);
        right->print_node();
        indentLevel--;
    }
};

class LogicExpr : public Expr{
    LogicOp op;
    Expr* left;
    Expr* right;
public:
    LogicExpr(LogicOp _op, Expr* _left, Expr* _right) : op(_op), left(_left), right(_right) {}
    void print_node(){
        std::string subType;
        switch(op){
            case LogicOp::AND : subType = "AND"; break;
            case LogicOp::OR : subType = "OR"; break;
           
        }
        print_text("LogicExpr("+ subType +"):");
        indentLevel++;
        print_text("left: ", false);
        left->print_node();
        print_text("right: ", false);
        right->print_node();
        indentLevel--;
    }
};

//a = b = 4
class AssignExpr : public Expr{
    Expr* left;
    Expr* right;
    public:
    AssignExpr(Expr* _left, Expr* _right) : left(_left), right(_right){}
    void print_node(){
        print_text("AssignExpr:");
        indentLevel++;
        print_text("left: ", false);
        left->print_node();
        print_text("right: ", false);
        right->print_node();
        indentLevel--;
    }
};


class ArraySubscriptExpr: public Expr{
    Expr* array; 
    Expr* index;
public:
    ArraySubscriptExpr(Expr* _array, Expr* _index) : array(_array), index(_index) {}
};

class FnCallExpr : public Expr{
    std::string toCall; 
    std::vector<Expr*> callArgs;
public:
    FnCallExpr(std::string _toCall, std::vector<Expr*> _callParams) : toCall(_toCall), callArgs(_callParams) {}
};

class FieldAccessExpr : public Expr{
    Expr* expr;
    std::string member;
public:
    FieldAccessExpr(Expr* _expr, std::string _member) : expr(_expr), member(_member) {}
};

class IncDecExpr : public Expr{
    IncDecOp op;
    std::string expr;
public:
    IncDecExpr(IncDecOp _op, std::string _expr) : op(_op), expr(_expr) {}
};