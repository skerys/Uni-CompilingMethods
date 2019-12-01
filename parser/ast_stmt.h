#pragma once

#include "ast_expr.h"

class Stmt : public Node{
    public:
};

class StmtBlock : public Node{
    std::vector<Stmt*> statements;

    public:
    StmtBlock(std::vector<Stmt*> _statements) : statements(_statements){}

    void print_node(){
        print_text("StmtBlock:");
        indentLevel++;
        for(int i = 0; i < statements.size(); i++){
            print_text("statement["+stringulate(i)+"]:", false);
            statements[i]->print_node();
        }
        indentLevel--;
    }
};  



class ExprStmt: public Stmt{
    Expr* expr;
    public:
    ExprStmt(Expr* _expr) : expr(_expr) {}
    void print_node(){
        print_text("ExprStmt:");
        indentLevel++;
        print_text("expr: ", false);
        expr->print_node();
        indentLevel--;
    }
};

class IfElseStmt : public Stmt{
    std::vector<std::pair<Expr*, StmtBlock*>> branches;
    StmtBlock* elseBody;

    public:
    IfElseStmt(std::vector<std::pair<Expr*, StmtBlock*>> _branches, StmtBlock* _elseBody):
    branches(_branches), elseBody(_elseBody) {}
    void print_node(){
        print_text("IfElseStmt:");
        indentLevel++;
        for(int i = 0; i < branches.size(); i++){
            print_text("branch[" + stringulate(i) + "]: ", false);
            print_text("condition: ", false);
            branches[i].first->print_node();
            print_text("body: ", false);
            branches[i].second->print_node();
        }
        print_text("elseBody: ", false);
        elseBody->print_node();
        indentLevel--;
    }
};

class WhileStmt : public Stmt{
    Expr* condition;
    StmtBlock* body;
public:
    WhileStmt(Expr* _condition, StmtBlock* _body) : condition(_condition), body(_body) {}
    void print_node(){
        print_text("WhileStmt:");
        indentLevel++;
        print_text("condition: ", false);
        condition->print_node();
        print_text("body: ", false);
        body->print_node();
        indentLevel--;
    }
};

class ForStmt : public Stmt{
    Stmt* initial;
    Stmt* condition;
    Stmt* final;
    StmtBlock* body;
public:
    ForStmt(Stmt* _initial, Stmt* _condition, Stmt* _final, StmtBlock* _body) : initial(_initial), condition(_condition), final(_final), body(_body){}
    void print_node(){
        print_text("ForStmt:");
        indentLevel++;
        print_text("initial: ", false);
        initial->print_node();
        print_text("condition: ", false);
        condition->print_node();
        print_text("final: ", false);
        final->print_node();
        print_text("body: ", false);
        body->print_node();
        indentLevel--;
    }

};

class ReturnStmt : public Stmt{
    public:
    Token* keyword;
    Expr* value;
public:
    ReturnStmt(Expr* _value, Token* _keyword) : value(_value), keyword(_keyword) {}
    void print_node(){
        print_text("ReturnStmt:");
        indentLevel++;
        print_text("keyword: " + std::get<std::string>(keyword->value));
        print_text("value: ", false);
        if(value != nullptr){
            value->print_node();    
        }    
        indentLevel--;
    }
};

class BreakStmt : public Stmt{
public:
    Token* keyword;
    BreakStmt(Token* _keyword) : keyword(_keyword){}
    void print_node(){
        print_text("BreakStmt:");
        indentLevel++;
        print_text("keyword: " + std::get<std::string>(keyword->value));
        indentLevel--;
    }
};
class NextStmt : public Stmt{
public:
    Token* keyword;
    NextStmt(Token* _keyword) : keyword(_keyword){}
    void print_node(){
        print_text("NextStmt:");
        indentLevel++;
        print_text("keyword: " + std::get<std::string>(keyword->value));
        indentLevel--;
    }
};

class InputStmt : public Stmt{
    std::vector<VarExpr*> idents;
    public:
    InputStmt(std::vector<VarExpr*> _idents) : idents(_idents) {} 

    void print_node(){
        print_text("InputStmt:");
        indentLevel++;
        for(int i = 0; i < idents.size(); i++){
            idents[i]->print_node();
        }
        indentLevel--;
    }

};

class OutputStmt : public Stmt{
    std::vector<Expr*> exprs;
    public:
    OutputStmt(std::vector<Expr*> _exprs) : exprs(_exprs){}

    void print_node(){
        print_text("OutputStmt:");
        indentLevel++;
        for(int i = 0; i < exprs.size(); i++){
            print_text("expr[" + stringulate(i) + "]: ",false);
            exprs[i]->print_node();
        }
        indentLevel--;
    }
};

class DeclareVarStmt : public Stmt{
    Type* type;
    Token* ident;
    Expr* assignExpr;
    public:
    DeclareVarStmt(Type* _type, Token* _ident, Expr* _assignExpr) : type(_type), ident(_ident), assignExpr(_assignExpr){}
    void print_node(){
        print_text("DeclareStmt:");
        indentLevel++;
        print_text("type: " + type->get_type_name());
        print_text("ident: " + std::get<std::string>(ident->value));

        if(assignExpr != nullptr){
            print_text("assignExpr:", false);
            assignExpr->print_node();
        }
        indentLevel--;
    }
};

//DeclareStmtArray


