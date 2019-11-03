#include "../lexer/lexer.h"
#include <vector>
#include <string>
#include <memory>

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






int main()
{
    lex_file("grammar/code_sample.txt");
}