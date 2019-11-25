#include "../lexer/lexer.h"
#include <vector>
#include <string>
#include <memory>

#include "ast_decl.h"

class Parser{
public:
    int offset;
    std::vector<Token> tokens;
    Token currentToken;
    bool running;
    

    Parser(std::vector<Token> _tokens) : offset(0), tokens(_tokens) {};

    Token* accept(TokenType type){
        currentToken = tokens[offset];
        if(currentToken.type == type){
            offset++;
            
            return &currentToken;
        }
        return nullptr;
    }

    Token* expect(TokenType type){
        currentToken = tokens[offset];
        if(currentToken.type == type){
            offset++;
           
            return &currentToken;
        } else{
            
            running = false;
            return nullptr;
        }
    }

    TokenType token_type()
    {
         return tokens[offset].type;
    }

    bool peek_two(TokenType first, TokenType second){
        return tokens[offset].type == first && tokens[offset+1].type == second;
    }

    Type* parseType(){
        switch(token_type()){
            case TokenType::KW_BOOL: expect(TokenType::KW_BOOL); return "bool";
            case TokenType::KW_FLOAT: expect(TokenType::KW_FLOAT); return "float";
            case TokenType::KW_INT: expect(TokenType::KW_INT); return "int";
            case TokenType::KW_STRING: expect(TokenType::KW_STRING); return "string";
            case TokenType::KW_VOID: expect(TokenType::KW_VOID); return "void";
            case TokenType::IDENT: return std::get<std::string>(expect(TokenType::IDENT)->value);
            }
    }

    Param* parse_param()
    {
        auto retType = parseType();
        auto name = expect(TokenType::IDENT);
        return new Param(retType, name);
    }

    std::vector<Param*> parse_params(){
        std::vector<Param*> params;
        expect(TokenType::OP_PAREN_OPEN);

        TokenType typeNow = token_type();
        if(typeNow != TokenType::IDENT && typeNow != TokenType::KW_INT && typeNow != TokenType::KW_STRING && typeNow != TokenType::KW_BOOL && typeNow != TokenType::KW_FLOAT){
            expect(TokenType::OP_PAREN_CLOSE);
            return params;
        }

        params.push_back(parse_param());
        while(accept(TokenType::OP_COMMA_SEP) != nullptr){
            params.push_back(parse_param());
        }
        expect(TokenType::OP_PAREN_CLOSE);
        return params;
    }

    Expr* parse_expr_priority(){
        expect(TokenType::OP_PAREN_OPEN);
        auto expr = parse_expr();
        expect(TokenType::OP_PAREN_CLOSE);
        auto p = new ParenExpr(expr);
        return p;
    }

    Expr* parse_expr_call(){
        std::string name = std::get<std::string>(expect(TokenType::IDENT)->value);
        std::vector<Expr*> args;
        expect(TokenType::OP_PAREN_OPEN);
        if(currentToken.type != TokenType::OP_PAREN_CLOSE){
            args.push_back(parse_expr());
        }
        while (currentToken.type != TokenType::OP_PAREN_CLOSE){
            expect(TokenType::OP_COMMA_SEP);
            args.push_back(parse_expr());
        }
        expect(TokenType::OP_PAREN_CLOSE);
        return new FnCallExpr(name, args);
    }

    Expr* parse_expr_primary(){

        if(peek_two(TokenType::IDENT, TokenType::OP_PAREN_OPEN)){
            return parse_expr_call();
        }
        else if(peek_two(TokenType::IDENT, TokenType::OP_INCREMENT)){
            std::string name = std::get<std::string>(expect(TokenType::IDENT)->value);
            expect(TokenType::OP_INCREMENT);
            return new IncDecExpr(IncDecOp::INC, name);
        }
        else if(peek_two(TokenType::IDENT, TokenType::OP_DECREMENT)){
            std::string name = std::get<std::string>(expect(TokenType::IDENT)->value);
            expect(TokenType::OP_DECREMENT);
            return new IncDecExpr(IncDecOp::DEC, name);
        }
        

        switch(currentToken.type){
            case TokenType::LIT_INT : return new LitExpr(std::get<int>(expect(TokenType::LIT_INT)->value));
            case TokenType::LIT_STR : return new LitExpr(std::get<std::string>(expect(TokenType::LIT_STR)->value));
            case TokenType::LIT_FLOAT : return new LitExpr(std::get<float>(expect(TokenType::LIT_FLOAT)->value));
            case TokenType::IDENT :  return new VarExpr(expect(TokenType::IDENT));
            case TokenType::OP_PAREN_OPEN : return parse_expr_priority();
            default : return nullptr;
        }
    }


    Expr* parse_expr_unary()
    {
        Expr* expr = nullptr;
        while(1){
            if(accept(TokenType::OP_PLUS) != nullptr){
                expr = new UnaryExpr(UnaryOp::POSITIVE, parse_expr_unary());
            }
            else if(accept(TokenType::OP_MINUS) != nullptr){
                expr = new UnaryExpr(UnaryOp::NEGATIVE, parse_expr_unary());
            }
            else if(accept(TokenType::OP_LOGIC_NOT) != nullptr){
                expr = new UnaryExpr(UnaryOp::NOT, parse_expr_unary());
            }
            else
                if(expr != nullptr)
                {
                    return expr;   
                }else{
                    return parse_expr_primary();
                }
        }
    }

    Expr* parse_expr_mult()
    {
        auto left = parse_expr_unary();
        while(1){
            if(accept(TokenType::OP_MULT) != nullptr){
                left = new ArithExpr(ArithOp::MUL, left, parse_expr_unary());
            }
            else if(accept(TokenType::OP_DIV) != nullptr){
                left = new ArithExpr(ArithOp::DIV, left, parse_expr_unary());
            }
            else
                return left;
        }
    }

    Expr* parse_expr_add()
    {
        auto left = parse_expr_mult();
        while(1){
            if(accept(TokenType::OP_PLUS) != nullptr){
                left = new ArithExpr(ArithOp::ADD, left, parse_expr_mult());

            }
            else if(accept(TokenType::OP_MINUS) != nullptr){
                left = new ArithExpr(ArithOp::SUB, left, parse_expr_mult());
            }
            else
                return left;
        }
    }

    Expr* parse_expr_compare(){
        auto left = parse_expr_add();

        while(1){
            if(accept(TokenType::OP_L) != nullptr){
                left = new CompareExpr(CompareOp::L, left, parse_expr_add());
            }
            else if (accept(TokenType::OP_LE) != nullptr){
                left = new CompareExpr(CompareOp::LE, left, parse_expr_add());
            }
            else if (accept(TokenType::OP_H) != nullptr){
                left = new CompareExpr(CompareOp::G, left, parse_expr_add());
            }
            else if (accept(TokenType::OP_HE) != nullptr){
                left = new CompareExpr(CompareOp::GE, left, parse_expr_add());
            }
            else if (accept(TokenType::OP_E) != nullptr){
                left = new CompareExpr(CompareOp::E, left, parse_expr_add());
            }
            else
                return left;
        }
    }

    Expr* parse_expr_logic()
    {
        auto left = parse_expr_compare();
        while(1){
            if(accept(TokenType::OP_LOGIC_AND) != nullptr){
                left = new LogicExpr(LogicOp::AND, left, parse_expr_compare());

            }
            else if(accept(TokenType::OP_LOGIC_OR) != nullptr){
                left = new LogicExpr(LogicOp::OR, left, parse_expr_compare());
            }
            else
                return left;
        }
    }


    Expr* parse_expr()
    {
        return parse_expr_logic();
    }

    Stmt* parse_stmt_return(){
        expect(TokenType::KW_RETURN);
        //auto value = token_type() != TokenType::OP_NEWLINE_SEP ? parse_expr() : nullptr;
       

        return new ReturnStmt(parse_expr(), "return");
    }

    Stmt* parse_stmt_while()
    {
        expect(TokenType::KW_WHILE);
        expect(TokenType::OP_PAREN_OPEN);
        auto condition = parse_expr();
        expect(TokenType::OP_PAREN_CLOSE);
        auto body = parse_stmt_block();
        return new WhileStmt(condition, body);
    }

    Stmt* parse_stmt_for(){
        expect(TokenType::KW_FOR);
        auto initial = parse_stmt();
        expect(TokenType::OP_SEMICOLON_SEP);
        auto condition = parse_stmt();
        expect(TokenType::OP_SEMICOLON_SEP);
        auto final = parse_stmt();
        auto body = parse_stmt_block();
        return new ForStmt(initial, condition, final, body);
    }

    Stmt* parse_output_stmt(){
        std::vector<Expr*> exprs;
        expect(TokenType::KW_WRITE);

        exprs.push_back(parse_expr());

        while(1){
            if(accept(TokenType::OP_COMMA_SEP) != nullptr){
                exprs.push_back(parse_expr());
            }else{
                return new OutputStmt(exprs);
            }
        }
    }

    Stmt* parse_input_stmt(){
        std::vector<std::string> idents;
        expect(TokenType::KW_READ);

        idents.push_back(std::get<std::string>(expect(TokenType::IDENT)->value));

        while(1){
            if(accept(TokenType::OP_COMMA_SEP) != nullptr){
                idents.push_back(std::get<std::string>(expect(TokenType::IDENT)->value));
            }else{
                return new InputStmt(idents);
            }
        }
    }

    Stmt* parse_expr_stmt(){
        return new ExprStmt(parse_expr());
    }

    Stmt* parse_stmt_elif(){
        expect(TokenType::KW_IF);
        std::vector<std::pair<Expr*, StmtBlock*>> elifStmts;

        std::pair<Expr*, StmtBlock*> pair;
        pair.first = parse_expr();
        pair.second = parse_stmt_block();
        elifStmts.push_back(pair);
        StmtBlock* elseBody = nullptr;

        while(1){
            if(accept(TokenType::KW_ELIF) != nullptr){
                std::pair<Expr*, StmtBlock*> pair;
                pair.first = parse_expr();
                pair.second = parse_stmt_block();
                elifStmts.push_back(pair);
            }
            else if(accept(TokenType::KW_ELSE) != nullptr){
                elseBody = parse_stmt_block();
            }
            else{
                return new IfElseStmt(elifStmts, elseBody);
            }
        }

    }

    Stmt* parse_stmt_assign(){
        std::string name = std::get<std::string>(expect(TokenType::IDENT)->value);
        expect(TokenType::OP_ASSIGN);
        
        if(peek_two(TokenType::IDENT, TokenType::OP_ASSIGN)){
            return new AssignStmt(name, parse_stmt_assign());    
        }

        return new AssignStmt(name, parse_expr_stmt());
        
    }

    Stmt* parse_stmt_declare()
    {
        auto type = parseType();
        std::string name = std::get<std::string>(expect(TokenType::IDENT)->value);
        std::cout <<"YAY" << std::endl;
        if(accept(TokenType::OP_ASSIGN)!= nullptr){
            return new DeclareStmt(type, name, parse_expr());
        }else{
            return new DeclareStmt(type, name, nullptr);
        }
    }
    Stmt* parse_stmt(){
        //std::cout << TokenNames[token_type()] <<std::endl;
        if(peek_two(TokenType::IDENT, TokenType::OP_ASSIGN)){
            return parse_stmt_assign();
        }

        if(peek_two(TokenType::IDENT, TokenType::IDENT)){
            return parse_stmt_declare();
        }

        switch(token_type()){
            case TokenType::KW_BOOL:
            case TokenType::KW_INT:
            case TokenType::KW_FLOAT:
            case TokenType::KW_STRING: return parse_stmt_declare();
            case TokenType::KW_IF : return parse_stmt_elif();
            case TokenType::KW_RETURN : return parse_stmt_return();
            case TokenType::KW_WHILE : return parse_stmt_while();
            case TokenType::KW_FOR : return parse_stmt_for();
            case TokenType::KW_WRITE : return parse_output_stmt();
            case TokenType::KW_READ : return parse_input_stmt();
            case TokenType::KW_NEXT : 
                expect(TokenType::KW_NEXT);
                return new NextStmt("next");
            case TokenType::KW_BREAK : 
                expect(TokenType::KW_BREAK);
                return new BreakStmt("break");
            default : return parse_expr_stmt();
        }
        
    }

    StmtBlock* parse_stmt_block(){
        std::vector<Stmt*> stmts;
        expect(TokenType::OP_CB_OPEN);

        while(1){
            if(accept(TokenType::OP_CB_CLOSE) != nullptr){
                break;
            }else{
                stmts.push_back(parse_stmt());
            }
        }
        return new StmtBlock(stmts);
    }

    Decl* parse_func_decl(){
        expect(TokenType::KW_FUNC);
        auto retType = parseType();
        auto name = std::get<std::string>(expect(TokenType::IDENT)->value);
        auto params = parse_params();
        auto body = parse_stmt_block();

        
        return new FnDecl(retType, name, params, body);
        

    }

    Decl* parse_decl(){
        switch(currentToken.type){
            case TokenType::KW_FUNC : return parse_func_decl();
            case TokenType::KW_CLASS : //return parse_class_decl();
            default:
                std::cout << "expected a 'func' or 'class'" << std::endl;
                running = false;
        }
    }

    Program* parse_program(){
        Program* p = new Program();

        while(running){
            if(accept(TokenType::EOF_) != nullptr) break;
            
            else{
                p->addDecl(parse_decl());
            }
        }
        return p;
    };
};


int main(int argc, char** argv){
    if(argc != 2){
        std::cout << "usage: tm <code_path>" << std::endl;
        return 1;
    }
    std::vector<Token> tokens = lex_file(argv[1], true);
    Parser p(tokens);
    Program* prog = p.parse_program();
    prog->print_node();
    

}