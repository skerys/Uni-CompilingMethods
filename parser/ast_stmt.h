#pragma once

#include "ast_expr.h"

class FnDecl;

class Stmt : public Node{
    public:
};

class StmtBlock : public Node{
    std::vector<Stmt*> statements;
    Scope* mScope;

    public:
    StmtBlock(std::vector<Stmt*> _statements) : statements(_statements){
        add_children(statements);
    }

    void print_node(){
        print_text("StmtBlock:");
        indentLevel++;
        for(int i = 0; i < statements.size(); i++){
            print_text("statement["+stringulate(i)+"]:", false);
            statements[i]->print_node();
        }
        indentLevel--;
    }

    void resolve_names(Scope* scope){
        mScope = new Scope(scope);
        for(auto&& s : statements){
            s->resolve_names(mScope);
        }
    }

    Type* check_types()
    {
        for(auto&& s : statements){
            s->check_types();
        }
        return nullptr;
    }

    void gen_code(CodeWriter w){
        for(auto&& s : statements){
            s->gen_code(w);
        }
    }
};  



class ExprStmt: public Stmt{
    Expr* expr;
    public:
    ExprStmt(Expr* _expr) : expr(_expr) {
        add_children(expr);
    }
    void print_node(){
        print_text("ExprStmt:");
        indentLevel++;
        print_text("expr: ", false);
        expr->print_node();
        indentLevel--;
    }
    void resolve_names(Scope* scope){
        expr->resolve_names(scope);
    }
    Type* check_types()
    {
        return expr->check_types();
    }
    void gen_code(CodeWriter w)
    {
        expr->gen_code(w);
        w.write(InstrName::I_POP);
    }
};

class IfElseStmt : public Stmt{
    std::vector<std::pair<Expr*, StmtBlock*>> branches;
    StmtBlock* elseBody;

    public:
    std::vector<Label*> endLabels;
    IfElseStmt(std::vector<std::pair<Expr*, StmtBlock*>> _branches, StmtBlock* _elseBody):
    branches(_branches), elseBody(_elseBody) {
        for(auto&& b : branches){
            add_children(b.first);
            add_children(b.second);
        }
        add_children(elseBody);
        //std::cout << "wow" << std::endl;
    }
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
        if(elseBody){
            print_text("elseBody: ", false);
            elseBody->print_node();
        }
        indentLevel--;
    }

    void resolve_names(Scope* scope){
        for(auto&& b : branches){
            b.first->resolve_names(scope);
            b.second->resolve_names(scope);
        }
        if(elseBody){
            elseBody->resolve_names(scope);
        }
    }


    Type* check_types()
    {
        auto boolType = new PrimitiveType(PrimitiveTypeName::BOOL);
        for(auto&& b : branches){
            Token* token = b.first->reference_token();
            unify_types(boolType, b.first->check_types(), token);
            b.second->check_types();
        }
        if(elseBody){
            elseBody->check_types();
        }
        return nullptr;
    }

    void gen_code(CodeWriter w){
        Label* lastLabel = new Label();
        for(int i = 0; i < branches.size(); i ++){
            endLabels.push_back(new Label());
            branches[i].first->gen_code(w);
            w.write_with_label(InstrName::I_BZ, endLabels[i]);
            branches[i].second->gen_code(w);
            w.write_with_label(InstrName::I_BR, lastLabel);
            w.place_label(endLabels[i]);
        }
        if(elseBody) elseBody->gen_code(w);
        w.place_label(lastLabel);
    }
};

class WhileStmt : public Stmt{
    Expr* condition;
    StmtBlock* body;
public:
    Label* endLabel;
    Label* startLabel;
    WhileStmt(Expr* _condition, StmtBlock* _body) : condition(_condition), body(_body) {
        add_children(condition);
        add_children(body);
        endLabel = new Label();
        startLabel = new Label();
    }
    void print_node(){
        print_text("WhileStmt:");
        indentLevel++;
        print_text("condition: ", false);
        condition->print_node();
        print_text("body: ", false);
        body->print_node();
        indentLevel--;
    }

    void resolve_names(Scope* scope){
        condition->resolve_names(scope);
        body->resolve_names(scope);
    }

    Type* check_types()
    {
        auto boolType = new PrimitiveType(PrimitiveTypeName::BOOL);
        unify_types(boolType, condition->check_types(), condition->reference_token());
        body->check_types();
        return nullptr;
    }

    void gen_code(CodeWriter w)
    {
        w.place_label(startLabel);
        condition->gen_code(w);
        w.write_with_label(InstrName::I_BZ, endLabel);
        body->gen_code(w);
        w.write_with_label(InstrName::I_BR, startLabel);
        w.place_label(endLabel);
    }
};

class ForStmt : public Stmt{
    Stmt* initial;
    Stmt* condition;
    Stmt* final;
    StmtBlock* body;
public:
    Label* endLabel;
    Label* startLabel;
    ForStmt(Stmt* _initial, Stmt* _condition, Stmt* _final, StmtBlock* _body) : initial(_initial), condition(_condition), final(_final), body(_body){
        add_children(initial);
        add_children(condition);
        add_children(final);
        add_children(body);
        endLabel = new Label();
        startLabel = new Label();
    }
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

    void resolve_names(Scope* scope){
        initial->resolve_names(scope);
        condition->resolve_names(scope);
        final->resolve_names(scope);
        body->resolve_names(scope);
    }

    Type* check_types()
    {
        auto boolType = new PrimitiveType(PrimitiveTypeName::BOOL);
        unify_types(boolType, condition->check_types(), condition->reference_token());
        body->check_types();
        return nullptr;
    }

    void gen_code(CodeWriter w)
    {
        initial->gen_code(w);
        w.place_label(startLabel);
        condition->gen_code(w);
        w.write_with_label(InstrName::I_BZ, endLabel);
        body->gen_code(w);
        final->gen_code(w);
        w.write_with_label(InstrName::I_BR, startLabel);
        w.place_label(endLabel);
    }

};

class ReturnStmt : public Stmt{
    public:
    Token* keyword;
    Expr* value;
public:
    ReturnStmt(Expr* _value, Token* _keyword) : value(_value), keyword(_keyword) {
        if(value) add_children(value);
    }
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

    void resolve_names(Scope* scope){
        if(value) value->resolve_names(scope);
    }

    Type* check_types()
    {
        Type* retType = ((Node*)find_ancestor<FnDecl>())->get_type();
        Type* valueType = nullptr;
        if(value != nullptr){
            valueType = value->check_types();
        }
        if(valueType == nullptr){
            valueType = new PrimitiveType(PrimitiveTypeName::VOID);
        }
        unify_types(retType, valueType, keyword);
        return retType;
    }

    void gen_code(CodeWriter w)
    {
        if(value)
        {
            value->gen_code(w);
            w.write(InstrName::I_RET_V);
        }
        else{
            w.write(InstrName::I_RET);
        }
    }
};

class BreakStmt : public Stmt{
public:
    Node* targetNode = nullptr;
    Token* keyword;
    Label* endLabel = nullptr;
    BreakStmt(Token* _keyword) : keyword(_keyword){}
    void print_node(){
        print_text("BreakStmt:");
        indentLevel++;
        print_text("keyword: " + std::get<std::string>(keyword->value));
        indentLevel--;
    }

    void resolve_names(Scope* scope){
        Node* currNode = parent;
        while(currNode){
            if(dynamic_cast<WhileStmt*>(currNode)){
                targetNode = currNode;
                endLabel = dynamic_cast<WhileStmt*>(currNode)->endLabel;
                break;
            }
            else if(dynamic_cast<ForStmt*>(currNode)){
                targetNode = currNode;
                endLabel = dynamic_cast<ForStmt*>(currNode)->endLabel;
                break;
            }
            else{
                currNode = currNode->parent;
            }
        }

        if(targetNode == nullptr){
            print_error(keyword, "'break' not in while or for statement\n");
        }
    }
    Type* check_types()
    {
        return nullptr;
    }

    void gen_code(CodeWriter w){
        w.write_with_label(InstrName::I_BR, endLabel);
    }
};
class NextStmt : public Stmt{
public:
    Node* targetNode = nullptr;
    Token* keyword;
    Label* startLabel = nullptr;
    NextStmt(Token* _keyword) : keyword(_keyword){}
    void print_node(){
        print_text("NextStmt:");
        indentLevel++;
        print_text("keyword: " + std::get<std::string>(keyword->value));
        indentLevel--;
    }

    void resolve_names(Scope* scope){
        Node* currNode = parent;
        while(currNode){
            if(dynamic_cast<WhileStmt*>(currNode)){
                targetNode = currNode;
                 startLabel = dynamic_cast<WhileStmt*>(currNode)->startLabel;
                break;
            }
            else if(dynamic_cast<ForStmt*>(currNode)){
                targetNode = currNode;
                 startLabel = dynamic_cast<ForStmt*>(currNode)->startLabel;
                break;
            }
            else{
                currNode = currNode->parent;
            }
        }

        if(targetNode == nullptr){
            //printf("error: next not in while or for statement: %d:%d", keyword->line_no, keyword->column_no);

            print_error(keyword, "'next' not in while or for statement\n");
        }
    }
    Type* check_types()
    {
        return nullptr;
    }

    void gen_code(CodeWriter w){
        w.write_with_label(InstrName::I_BR, startLabel);
    }
};

class InputStmt : public Stmt{
    std::vector<VarExpr*> idents;
    public:
    InputStmt(std::vector<VarExpr*> _idents) : idents(_idents) {
        add_children(idents);
    } 

    void print_node(){
        print_text("InputStmt:");
        indentLevel++;
        for(int i = 0; i < idents.size(); i++){
            idents[i]->print_node();
        }
        indentLevel--;
    }

    void resolve_names(Scope* scope){
        for(auto&& i : idents){
            i->resolve_names(scope);
        }
    }
    Type* check_types()
    {
        return nullptr;
    }

    void gen_code(CodeWriter w){
        for(auto&& i : idents){
            i->gen_code(w);
            auto type = dynamic_cast<PrimitiveType*>(i->check_types());
            switch (type->type)
            {
                case PrimitiveTypeName::INT : w.write(InstrName::I_READ_INT); break;
                case PrimitiveTypeName::FLOAT : w.write(InstrName::I_READ_FLOAT); break;
                case PrimitiveTypeName::STRING : w.write(InstrName::I_READ_STRING); break;
            }
        }
    }

};

class OutputStmt : public Stmt{
    std::vector<Expr*> exprs;
    public:
    OutputStmt(std::vector<Expr*> _exprs) : exprs(_exprs){
        add_children(exprs);
    }

    void print_node(){
        print_text("OutputStmt:");
        indentLevel++;
        for(int i = 0; i < exprs.size(); i++){
            print_text("expr[" + stringulate(i) + "]: ",false);
            exprs[i]->print_node();
        }
        indentLevel--;
    }

    void resolve_names(Scope* scope){
        for(auto&& e : exprs){
            e->resolve_names(scope);
        }
    }
    Type* check_types()
    {
        return nullptr;
    }

    void gen_code(CodeWriter w){
        for(auto&& e : exprs){
            e->gen_code(w);
            auto type = dynamic_cast<PrimitiveType*>(e->check_types());
            switch (type->type)
            {
                case PrimitiveTypeName::INT : w.write(InstrName::I_PRINT_INT); break;
                case PrimitiveTypeName::FLOAT : w.write(InstrName::I_PRINT_FLOAT); break;
                case PrimitiveTypeName::STRING : w.write(InstrName::I_PRINT_STRING); break;
            }
        }
    }
};

class DeclareVarStmt : public Stmt{
    Token* ident;
    Expr* assignExpr;
    public:
    Type* type;
    DeclareVarStmt(Type* _type, Token* _ident, Expr* _assignExpr) : type(_type), ident(_ident), assignExpr(_assignExpr){
        add_children(type);
        add_children(assignExpr);
    }
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

    void resolve_names(Scope* scope){
        scope->add(ident, this);
        stackSlot = stackSlotIndex;
        stackSlotIndex++;
    }
    Type* get_type()
    {
        return type;
    }

    Type* check_types()
    {
        if(assignExpr != nullptr)
        {
            auto valueType = assignExpr->check_types();
            unify_types(type, valueType, ident);
        }
        return nullptr;
    }

    void gen_code(CodeWriter w)
    {
        if(assignExpr != nullptr)
        {
            assignExpr->gen_code(w);
            w.write(InstrName::I_SET_L, stackSlot);
        }
    }
};

//DeclareStmtArray


