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
        } else{
            std::cout << "expected " << TokenNames[type] << "; got " << TokenNames[currentToken.type] << std::endl;
            running = false;
            return nullptr;
        }
    }

    TokenType token_type()
    {
         return tokens[offset].type;
    }

    std::string parseType(){
        switch(token_type()){
            case TokenType::KW_BOOL: expect(TokenType::KW_BOOL); return "bool";
            case TokenType::KW_FLOAT: expect(TokenType::KW_BOOL); return "float";
            case TokenType::KW_INT: expect(TokenType::KW_BOOL); return "int";
            case TokenType::KW_STRING: expect(TokenType::KW_BOOL); return "string";
            case TokenType::IDENT: return std::get<std::string>(expect(TokenType::IDENT)->value);
            }
    }

    std::unique_ptr<Param> parse_param()
    {
        auto retType = parseType();
        auto name = std::get<std::string>(expect(TokenType::IDENT)->value);
        return std::make_unique<Param>(retType, name);
    }

    std::vector<std::unique_ptr<Param>> parse_params(){
        std::vector<std::unique_ptr<Param>> params;

        expect(TokenType::OP_PAREN_OPEN);
        params.push_back(parse_param());
        while(accept(TokenType::OP_COMMA_SEP) != nullptr){
            params.push_back(parse_param());
        }
        return params;
    }

    std::unique_ptr<Expr> parse_expr_priority(){
        expect(TokenType::OP_PAREN_OPEN);
        auto expr = parse_expr();
        expect(TokenType::OP_PAREN_CLOSE);
        return std::make_unique<ParenExpr>(expr);
    }

    std::unique_ptr<Expr> parse_expr_primary(){
        switch(currentToken.type){
            case TokenType::LIT_INT : return std::make_unique<LitExpr>(std::get<int>(expect(TokenType::LIT_INT)->value));
            case TokenType::LIT_STR : return std::make_unique<LitExpr>(std::get<std::string>(expect(TokenType::LIT_STR)->value));
            case TokenType::LIT_FLOAT : return std::make_unique<LitExpr>(std::get<float>(expect(TokenType::LIT_FLOAT)->value));
            case TokenType::IDENT :  return std::make_unique<LitExpr>(std::get<std::string>(expect(TokenType::IDENT)->value));
            case TokenType::OP_PAREN_OPEN : return parse_expr_priority();
        }
    }

    std::unique_ptr<Expr> parse_expr_unary()
    {
        std::unique_ptr<Expr> expr = nullptr;
        while(1){
            if(accept(TokenType::OP_PLUS) != nullptr){
                expr = std::make_unique<UnaryExpr>(UnaryOp::POSITIVE, parse_expr_unary());
            }
            else if(accept(TokenType::OP_MINUS) != nullptr){
                expr = std::make_unique<UnaryExpr>(UnaryOp::NEGATIVE, parse_expr_unary());
            }
            else if(accept(TokenType::OP_LOGIC_NOT) != nullptr){
                expr = std::make_unique<UnaryExpr>(UnaryOp::NOT, parse_expr_unary());
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

    std::unique_ptr<Expr> parse_expr_mult()
    {
        auto left = parse_expr_unary();
        while(1){
            if(accept(TokenType::OP_MULT) != nullptr){
                left = std::make_unique<ArithExpr>(ArithOp::MUL, left, parse_expr_unary());
            }
            else if(accept(TokenType::OP_DIV) != nullptr){
                left = std::make_unique<ArithExpr>(ArithOp::DIV, left, parse_expr_unary());
            }
            else
                return left;
        }
    }

    std::unique_ptr<Expr> parse_expr_add()
    {
        auto left = parse_expr_mult();
        while(1){
            if(accept(TokenType::OP_PLUS) != nullptr){
                left = std::make_unique<ArithExpr>(ArithOp::ADD, left, parse_expr_mult());
            }
            else if(accept(TokenType::OP_MINUS) != nullptr){
                left = std::make_unique<ArithExpr>(ArithOp::SUB, left, parse_expr_mult());
            }
            else
                return left;
        }
    }

    std::unique_ptr<Expr> parse_expr_compare(){
        auto left = parse_expr_add();

        while(1){
            if(accept(TokenType::OP_L) != nullptr){
                left = std::make_unique<CompareExpr>(CompareOp::L, left, parse_expr_add());
            }
            else if (accept(TokenType::OP_LE) != nullptr){
                left = std::make_unique<CompareExpr>(CompareOp::LE, left, parse_expr_add());
            }
            else if (accept(TokenType::OP_H) != nullptr){
                left = std::make_unique<CompareExpr>(CompareOp::G, left, parse_expr_add());
            }
            else if (accept(TokenType::OP_HE) != nullptr){
                left = std::make_unique<CompareExpr>(CompareOp::GE, left, parse_expr_add());
            }
            else if (accept(TokenType::OP_E) != nullptr){
                left = std::make_unique<CompareExpr>(CompareOp::E, left, parse_expr_add());
            }
            else
                return left;
        }
    }


    std::unique_ptr<Expr> parse_expr()
    {
        return parse_expr_compare();
    }

    std::unique_ptr<Stmt> parse_stmt_return(){
        expect(TokenType::KW_RETURN);
        auto value = token_type() != TokenType::OP_NEWLINE_SEP ? parse_expr() : nullptr;
        expect(TokenType::OP_NEWLINE_SEP);

        return std::make_unique<ReturnStmt>(value); 
    }

    std::unique_ptr<Stmt> parse_stmt(){
        return parse_stmt_return();
    }

    std::vector<std::unique_ptr<Stmt>> parse_stmt_block(){
        std::vector<std::unique_ptr<Stmt>> stmts;
        expect(TokenType::OP_CB_OPEN);

        while(1){
            if(accept(TokenType::OP_CB_CLOSE) != nullptr){
                break;
            }else{
                stmts.push_back(parse_stmt());
            }
        }
        return stmts;
    }

    std::unique_ptr<Decl> parse_func_decl(){
        expect(TokenType::KW_FUNC);
        auto retType = parseType();
        auto name = std::get<std::string>(expect(TokenType::IDENT)->value);
        auto params = parse_params();
        auto body = parse_stmt_block();

        return std::move(std::make_unique<FnDecl>(retType, name, params, body);
        

    }

    std::unique_ptr<Decl> parse_decl(){
        switch(currentToken.type){
            case TokenType::KW_FUNC : return parse_func_decl();
            case TokenType::KW_CLASS : //return parse_class_decl();
            default:
                std::cout << "expected a 'func' or 'class'" << std::endl;
                running = false;
        }
    }

    void parse_program(){
        std::unique_ptr<Program> p = std::make_unique<Program>();

        while(running){
            if(accept(TokenType::EOF_) != nullptr) break;
            
            else{
                p->addDecl(parse_decl());
            }
        }
    };
};


int main(int argc, char** argv){
    if(argc != 2){
        std::cout << "usage: tm <code_path>" << std::endl;
        return 1;
    }
    std::vector<Token> tokens = lex_file(argv[1], true);
    Parser p(tokens);
    p.parse_program();

}