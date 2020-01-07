#pragma once

#include "ast_base.h"
#include <variant>
#include <algorithm>


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
    Node* target;
public:
    VarExpr(Token* _var) : var(_var){}
    void print_node(){
        print_text("VarExpr:");
        indentLevel++;
        print_text("name: " + std::get<std::string>(var->value));
        indentLevel--;
    }

    void resolve_names(Scope* scope){
        target = scope->resolve_name(var);
    }

    Token* reference_token()
    {
        return var;
    }

    Type* check_types();

    void gen_code(CodeWriter w){
        if(target->stackSlot != -1){
            w.write(InstrName::I_GET_L, target->stackSlot);
        }
        else{
            printf("bad variable varexp\n");
            exit(0);
        }
    }
};

class LitExpr : public Expr{
public:
    Token* litToken;
    LitExpr(Token* _litToken) : litToken(_litToken){}
void print_node(){
        print_text("LitExpr: ");
        indentLevel++;
        print_text("lit: ", false);
        std::visit(TokenValueVisitor{}, litToken->value);
        std::cout << std::endl;
        lastNl = true;
        indentLevel--;
    }
    void resolve_names(Scope* scope){
        //nothin
    }
    Token* reference_token()
    {
        return litToken;
    }

    Type* check_types()
    {
        switch(litToken->type){
            case TokenType::KW_FALSE :
            case TokenType::KW_TRUE : return new PrimitiveType(PrimitiveTypeName::BOOL);
            case TokenType::LIT_INT : return new PrimitiveType(PrimitiveTypeName::INT);
            case TokenType::LIT_STR : return new PrimitiveType(PrimitiveTypeName::STRING);
            case TokenType::LIT_FLOAT : return new PrimitiveType(PrimitiveTypeName::FLOAT);
        }
        return nullptr;
    }

    void gen_code(CodeWriter w){
        switch(litToken->type){
            case TokenType::KW_FALSE : w.write(InstrName::I_INT_PUSH, 0); break;
            case TokenType::KW_TRUE : w.write(InstrName::I_INT_PUSH, 1); break;
            case TokenType::LIT_INT : w.write(InstrName::I_INT_PUSH, std::get<int>(litToken->value)); break;
            //case TokenType::LIT_STR : return new PrimitiveType(PrimitiveTypeName::STRING);
            //case TokenType::LIT_FLOAT : return new PrimitiveType(PrimitiveTypeName::FLOAT);
        }
    }
};

class ParenExpr : public Expr{
    Expr* inside;
public:
    ParenExpr(Expr* _inside) : inside(_inside) {
        add_children(inside);
    }
    void print_node(){
        print_text("ParenExpr:");
        indentLevel++;
        print_text("inside: ", false);
        inside->print_node();
        indentLevel--;
    }
    void resolve_names(Scope* scope){
        //nothin
    }

    Token* reference_token()
    {
        return inside->reference_token();
    }

    Type* check_types()
    {
        return nullptr;
    }

    void gen_code(CodeWriter w){
        inside->gen_code(w);
    }
};

class UnaryExpr : public Expr{
    UnaryOp op;
    Expr* expr;
public:
    UnaryExpr(UnaryOp _op, Expr* _expr) : op(_op), expr(_expr) {
        add_children(expr);
    }
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
    void resolve_names(Scope* scope){
        expr->resolve_names(scope);
    }

    Token* reference_token()
    {
        return expr->reference_token();
    }

    Type* check_types()
    {
        return expr->check_types();
    }
    
};

class ArithExpr : public Expr{
    ArithOp op;
    Expr* left;
    Expr* right;
public:
    ArithExpr(ArithOp _op, Expr* _left, Expr* _right) : op(_op), left(_left), right(_right)
    {
        add_children(left);
        add_children(right);
    }
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
    void resolve_names(Scope* scope){
        left->resolve_names(scope);
        right->resolve_names(scope);
    }

    Token* reference_token()
    {
        return left->reference_token();
    }

    Type* check_types()
    {
        auto leftType = left->check_types();
        auto rightType = right->check_types();

        if(leftType->is_arithmethic()) {
            unify_types(leftType, rightType, left->reference_token());
        }
        else{
            print_error(reference_token(), "values of types '" + leftType->get_type_name() + "' and '" + rightType->get_type_name() + "' do not support arithmethic operations\n");
        }
        return leftType;
    }

    void gen_code(CodeWriter w){
        left->gen_code(w);
        right->gen_code(w);
        auto type = dynamic_cast<PrimitiveType*>(left->check_types());
        switch(op){
            case ArithOp::ADD:
                type->type == PrimitiveTypeName::FLOAT ? w.write(InstrName::I_FLOAT_ADD) : w.write(InstrName::I_INT_ADD);
                break;
            case ArithOp::SUB:
                type->type == PrimitiveTypeName::FLOAT ? w.write(InstrName::I_FLOAT_SUB) : w.write(InstrName::I_INT_SUB);
                break;
            case ArithOp::MUL:
                type->type == PrimitiveTypeName::FLOAT ? w.write(InstrName::I_FLOAT_MUL) : w.write(InstrName::I_INT_MUL);
                break;
            case ArithOp::DIV:
                type->type == PrimitiveTypeName::FLOAT ? w.write(InstrName::I_FLOAT_DIV) : w.write(InstrName::I_INT_DIV);
                break;

        }
    }
};

class CompareExpr : public Expr{
    CompareOp op;
    Expr* left;
    Expr* right;
public:
    CompareExpr(CompareOp _op, Expr* _left, Expr* _right) : op(_op), left(_left), right(_right) 
    {
        add_children(left);
        add_children(right);
    }
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
    void resolve_names(Scope* scope){
        left->resolve_names(scope);
        right->resolve_names(scope);
    }

    Token* reference_token()
    {
        return left->reference_token();
    }

    Type* check_types()
    {
        auto leftType = left->check_types();
        auto rightType = right->check_types();

        if(leftType->is_comparable()) {
            unify_types(leftType, rightType, left->reference_token());
        }
        else{
            print_error(reference_token(), "values of types '" + leftType->get_type_name() + "' and '" + rightType->get_type_name() + "' cannot be compared\n");
        }
        return new PrimitiveType(PrimitiveTypeName::BOOL);
    }

    void gen_code(CodeWriter w){
        left->gen_code(w);
        right->gen_code(w);
        switch(op){
            case CompareOp::E: w.write(InstrName::I_EQ); break;
            case CompareOp::G: w.write(InstrName::I_INT_GREATER); break;
            case CompareOp::GE: w.write(InstrName::I_INT_GREATER_EQ); break;
            case CompareOp::L: w.write(InstrName::I_INT_LESS); break;
            case CompareOp::LE: w.write(InstrName::I_INT_LESS_EQ); break;
            case CompareOp::NE: w.write(InstrName::I_NOT_EQ); break;
        }
    }
};

class LogicExpr : public Expr{
    LogicOp op;
    Expr* left;
    Expr* right;
public:
    LogicExpr(LogicOp _op, Expr* _left, Expr* _right) : op(_op), left(_left), right(_right) 
    {
        add_children(left);
        add_children(right);
    }
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

    Token* reference_token()
    {
        return left->reference_token();
    }

    void resolve_names(Scope* scope){
        left->resolve_names(scope);
        right->resolve_names(scope);
    }
    Type* check_types()
    {
        auto boolType = new PrimitiveType(PrimitiveTypeName::BOOL);
        auto leftType = left->check_types();
        auto rightType = right->check_types();

        unify_types(boolType, leftType, left->reference_token());
        unify_types(boolType, rightType, right->reference_token());
        return boolType;
    }
};

//a = b = 4
class AssignExpr : public Expr{
    Expr* left;
    Expr* right;
    Node* target;
    public:
    AssignExpr(Expr* _left, Expr* _right) : left(_left), right(_right)
    {
        add_children(left);
        add_children(right);
    }
    void print_node(){
        print_text("AssignExpr:");
        indentLevel++;
        print_text("left: ", false);
        left->print_node();
        print_text("right: ", false);
        right->print_node();
        indentLevel--;
    }
    void resolve_names(Scope* scope){
        left->resolve_names(scope);
        right->resolve_names(scope);
    }

    Token* reference_token()
    {
        return left->reference_token();
    }

    Type* check_types()
    {
        auto leftType = left->check_types();
        auto rightType = right->check_types();

        unify_types(leftType, rightType, reference_token());

        return leftType;
    }

    void gen_code(CodeWriter w){

    }


};


class FnCallExpr : public Expr{
    Token* toCall; 
    std::vector<Expr*> callArgs;
    Node* targetNode;
public:
    FnCallExpr(Token* _toCall, std::vector<Expr*> _callParams) : toCall(_toCall), callArgs(_callParams)
    {
        add_children(callArgs);
    }
    void print_node(){
        print_text("FnCallExpr:");
        indentLevel++;
        print_text("toCall: " + std::get<std::string>(toCall->value), false);
        for(int i = 0; i < callArgs.size(); i++){
            print_text("arg[" + stringulate(i) +"]:", false);
            callArgs[i]->print_node();
        }
        indentLevel--;
    }

    Token* reference_token(){
        return toCall;
    }

    void resolve_names(Scope* scope){
        targetNode = scope->resolve_name(toCall);
        for(auto&& a : callArgs){
            a->resolve_names(scope);
        }
    }

    Type* check_types();

    void gen_code(CodeWriter w){
        w.write(InstrName::I_CALL_BEGIN);
        for(auto&& a : callArgs){
            a->gen_code(w);
        }
        std::vector<int> argsize;
        argsize.push_back(callArgs.size());
        w.write(InstrName::I_CALL, targetNode->get_start_label(), argsize);
    }

};


class IncDecExpr : public Expr{
    IncDecOp op;
    std::string expr;
public:
    IncDecExpr(IncDecOp _op, std::string _expr) : op(_op), expr(_expr) {}
};