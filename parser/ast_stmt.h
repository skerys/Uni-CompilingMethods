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
        std::cout << std::endl;
        indentLevel--;
    }
};  

//a = b = 4
class AssignStmt : public Stmt{
    std::string name;
    Stmt* stmt;
    public:
    AssignStmt(std::string _name, Stmt* _stmt) : name(_name), stmt(_stmt) {}
    void print_node(){
        print_text("AssignStmt:");
        indentLevel++;
        print_text("name: " + name);
        print_text("stmt: ", false);
        stmt->print_node();
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
    Expr* condition;
    StmtBlock* body;
    std::vector<std::pair<Expr*, StmtBlock*>> elifStmts;
    StmtBlock* elseBody;

    public:
    IfElseStmt(Expr* _condition, StmtBlock* _body, std::vector<std::pair<Expr*, StmtBlock*>> _elifStmts, StmtBlock* _elseBody):
    condition(_condition), body(_body), elifStmts(_elifStmts), elseBody(_elseBody) {}
    void print_node(){
        print_text("IfElseStmt:");
        indentLevel++;
        print_text("condition: ", false);
        condition->print_node();
        print_text("body: ", false);
        body->print_node();
        for(int i = 0; i < elifStmts.size(); i++){
            print_text("branch[" + stringulate(i) + "]: ", false);
            print_text("condition: ", false);
            elifStmts[i].first->print_node();
            print_text("body: ", false);
            elifStmts[i].second->print_node();
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
    Expr* value;
public:
    ReturnStmt(Expr* _value) : value(_value) {}
    void print_node(){
        print_text("ReturnStmt:");
        indentLevel++;
        print_text("value: ", false);
        if(value != nullptr){
            value->print_node();    
        }    
        indentLevel--;
    }
};

class BreakStmt : public Stmt{
public:
    void print_node(){
        print_text("BreakStmt:");
    }
};
class NextStmt : public Stmt{
    public:
    void print_node(){
        print_text("NextStmt:");
    }
};

class InputStmt : public Stmt{
    std::vector<std::string> idents;
    public:
    InputStmt(std::vector<std::string> _idents) : idents(_idents) {} 

    void print_node(){
        print_text("InputStmt:");
        indentLevel++;
        for(int i = 0; i < idents.size(); i++){
            print_text("ident[" + stringulate(i) + "]: " + idents[i]);
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

class DeclareStmt : public Stmt{
    std::string type;
    std::string ident;
    Expr* assignExpr;
    public:
    DeclareStmt(std::string _type, std::string _ident, Expr* _assignExpr) : type(_type), ident(_ident), assignExpr(_assignExpr){}
    void print_node(){
        print_text("DeclareStmt:");
        indentLevel++;
        print_text("type: " + type);
        print_text("ident: " + ident);

        if(assignExpr != nullptr){
            print_text("assignExpr:", false);
            assignExpr->print_node();
        }
        indentLevel--;
    }
};

class DeclareStmtArray : public Stmt{
    std::string type;
    std::string ident;
    Expr* sizeExpr;
    std::vector<Expr*> assignExprs;
};


