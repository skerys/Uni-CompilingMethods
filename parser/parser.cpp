#include "../lexer/lexer.h"
#include <vector>
#include <string>
#include <memory>

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

enum AccesibilityName{
    PUBLIC, PRIVATE
};

class Node{
    virtual void print_node(){
        std::cout << "print node not implemented for type " << typeid(this).name() << std::endl;
    }
};

class Param : Node{
    std::string name;
    std::string type;
};

class Stmt : Node{

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

class Decl : Node{

};

class FnDecl : Decl{
    std::string name;
    std::string returnType;
    std::vector<std::unique_ptr<Param>> params;
    StmtBlock body;

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






int main()
{
    lex_file("grammar/code_sample.txt");
}