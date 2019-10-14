#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <fstream>
#include <cerrno>


enum TokenType{
    IDENT, LIT_INT, LIT_FLOAT, LIT_STR, OP_PLUS, OP_INCREMENT, OP_MINUS, OP_DECREMENT, OP_L, OP_LE, OP_H, OP_HE, OP_ASSIGN, OP_E,
    OP_PAREN_OPEN, OP_PAREN_CLOSE, OP_CB_OPEN, OP_CB_CLOSE, OP_SB_OPEN, OP_SB_CLOSE, OP_MULT, OP_DIV, OP_LOGIC_AND, OP_LOGIC_OR, OP_LOGIC_NOT, OP_FIELD_ACCESS, OP_COMMA_SEP, OP_SEMICOLON_SEP,
    KW_IF, KW_ELIF, KW_ELSE, KW_WHILE, KW_FOR, KW_RETURN, KW_BREAK, KW_NEXT, KW_READ, KW_WRITE, KW_CLASS, KW_PRIVATE, KW_PUBLIC, KW_FUNC, KW_INT, KW_FLOAT, KW_BOOL, KW_STRING, KW_VOID, KW_TRUE, KW_FALSE, EOF_
};

const std::string TokenNames[] = {"IDENT", "LIT_INT", "LIT_FLOAT", "LIT_STR", "OP_PLUS", "OP_INCREMENT", "OP_MINUS", "OP_DECREMENT", "OP_L", "OP_LE", "OP_H", "OP_HE", "OP_ASSIGN", "OP_E",
                                  "OP_PAREN_OPEN", "OP_PAREN_CLOSE", "OP_CB_OPEN", "OP_CB_CLOSE", "OP_SB_OPEN", "OP_SB_CLOSE", "OP_MULT", "OP_DIV", "OP_LOGIC_AND", "OP_LOGIC_OR", "OP_LOGIC_NOT",
                                  "OP_FIELD_ACCESS", "OP_COMMA_SEP", "OP_SEMICOLON_SEP",
                                  "KW_IF", "KW_ELIF", "KW_ELSE", "KW_WHILE", "KW_FOR", "KW_RETURN", "KW_BREAK", "KW_NEXT", "KW_READ", "KW_WRITE", "KW_CLASS", "KW_PRIVATE", "KW_PUBLIC",
                                  "KW_FUNC", "KW_INT", "KW_FLOAT", "KW_BOOL", "KW_STRING", "KW_VOID", "KW_TRUE", "KW_FALSE", "EOF"};

const std::map<std::string, TokenType> keywords = {
    {"if", TokenType::KW_IF},
    {"elif", TokenType::KW_ELIF},
    {"else", TokenType::KW_ELSE},
    {"while", TokenType::KW_WHILE},
    {"for", TokenType::KW_FOR},
    {"return", TokenType::KW_RETURN},
    {"break", TokenType::KW_BREAK},
    {"next", TokenType::KW_NEXT},
    {"read", TokenType::KW_READ},
    {"write", TokenType::KW_WRITE},
    {"class", TokenType::KW_CLASS},
    {"private", TokenType::KW_PRIVATE},
    {"public", TokenType::KW_PUBLIC},
    {"func", TokenType::KW_FUNC},
    {"int", TokenType::KW_INT},
    {"float", TokenType::KW_FLOAT},
    {"bool", TokenType::KW_BOOL},
    {"string", TokenType::KW_STRING},
    {"void", TokenType::KW_VOID},
    {"true", TokenType::KW_TRUE},
    {"false", TokenType::KW_FALSE}
};

struct Token{
    int id;
    TokenType type;
    std::string value;
    int line_no;
    int column_no;

    Token(){}
    Token(TokenType _type, std::string _value, int _line_no) : type(_type), value(_value), line_no(_line_no) {}
};

enum State{
    S_START, S_IDENT, S_LIT_INT, S_LIT_FLOAT, S_LIT_FLOAT_EXP_NEG, S_LIT_FLOAT_EXP, S_LIT_STR, S_ESCAPE_CHAR,
    S_OP_PLUS, S_OP_MINUS, S_OP_LOW, S_OP_HIGH, S_OP_EQUAL, S_OP_OR, S_OP_AND, S_OP_DOT, S_OP_SLASH, S_COMMENT, S_MULTILINECOMMENT, S_MULTILINECOMMENT_EXIT
};

std::string get_file_contents(const char* filename){
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if(in){
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw(errno);
}


class Lexer{
public:
    std::string input;
    int offset = 0;

    char current_char;
    State current_state = State::S_START;
    std::string current_lexeme;

    int current_line = 1;
    int current_column = 1;
    int token_id = 0;
    bool running = true;

    int lexeme_start;

    std::vector<Token> tokens;

    void print_tokens(){
            std::cout << std::left << std::setw(5) << std::setfill(' ') << "ID" << '|';
            std::cout << std::left << std::setw(5) << std::setfill(' ') << "LINE" << '|';
            std::cout << std::left << std::setw(5) << std::setfill(' ') << "COL" << '|';
            std::cout << std::left << std::setw(17) << std::setfill(' ') << "TOKEN" << '|';
            std::cout << std::left << std::setw(20) << std::setfill(' ') << "VALUE" << '|';
            std::cout << std::endl;
            std::cout << "----------------------------------------------------" <<std::endl;

        for(Token t : tokens){
            std::cout << std::left << std::setw(5) << std::setfill(' ') << t.id << '|';
            std::cout << std::left << std::setw(5) << std::setfill(' ') << t.line_no << '|';
            std::cout << std::left << std::setw(5) << std::setfill(' ') << t.column_no << '|';
            std::cout << std::left << std::setw(17) << std::setfill(' ') << TokenNames[t.type] << '|';
            std::cout << std::left << std::setw(20) << std::setfill(' ') << t.value << '|';
            std::cout << std::endl;
        }
    }

    void change_state(State new_state)
    {
        current_state = new_state;
        current_lexeme += current_char;
    }

    void change_state(State new_state, bool add_to_lexeme){
        current_state = new_state;
        if(add_to_lexeme)
        {
            current_lexeme += current_char;
        }
    }

    void complete_token(TokenType type, bool decrement){
        complete_token(type, decrement, true);
    }

    void complete_token(TokenType type, bool decrement, bool with_value){
        if(type == TokenType::IDENT){
            auto kwPair = keywords.find(current_lexeme);
            if(kwPair != keywords.end()){
                type = kwPair->second;
                current_lexeme="";
            }
        }
        Token toAdd;
        toAdd.type = type;
        toAdd.value = with_value ? current_lexeme : "";
        toAdd.line_no = current_line;
        toAdd.id = token_id;
        toAdd.column_no = lexeme_start;

        tokens.push_back(toAdd);

        token_id++;
        current_state = State::S_START;
        current_lexeme = "";

        if(decrement){
            offset--;
            current_column--;
        }
    }

    void add_to_lexeme(){
        current_lexeme += current_char;
    }

    void add_to_lexeme(char c){
        current_lexeme += c;
    }

    void lex_all(){
        while(offset < input.size()){
            if(!running) break;
            current_char = input[offset];
            lex();
            offset++;
            current_column++;
        }

        if(running)
        {
            
            current_char=' ';
            lex();

            if(current_state != State::S_START){
                printf("lexer error at %d:%d : unterminated token\n", current_line, current_column);
                running = false;
            }else{
                complete_token(TokenType::EOF_, false, false);
            }

        }
        if(running) print_tokens();
    }

    void lex(){
        switch(current_state){
            case State::S_START: lex_start(); break;
            case State::S_IDENT: lex_ident(); break;
            case State::S_LIT_INT: lex_lit_int(); break;
            case State::S_LIT_FLOAT: lex_lit_float(); break;
            case State::S_LIT_FLOAT_EXP: lex_lit_float_exp(); break;
            case State::S_LIT_FLOAT_EXP_NEG: lex_lit_float_exp_neg(); break;
            case State::S_LIT_STR : lex_lit_str(); break;
            case State::S_ESCAPE_CHAR: lex_escape_char(); break;
            case State::S_OP_PLUS: lex_op_plus(); break;
            case State::S_OP_MINUS: lex_op_minus(); break;
            case State::S_OP_HIGH: lex_op_high(); break;
            case State::S_OP_LOW: lex_op_low(); break;
            case State::S_OP_EQUAL: lex_op_equal(); break;
            case State::S_OP_DOT: lex_op_dot(); break;
            case State::S_OP_AND: lex_op_and(); break;
            case State::S_OP_OR: lex_op_or(); break;
            case State::S_OP_SLASH: lex_op_slash(); break;
            case State::S_COMMENT: lex_comment(); break;
            case State::S_MULTILINECOMMENT: lex_multiline(); break;
            case State::S_MULTILINECOMMENT_EXIT: lex_m_com_exit(); break;
        }
    }

    void lex_op_slash(){
        switch(current_char)
        {
            case '#': change_state(State::S_MULTILINECOMMENT, false);break;
            default: complete_token(TokenType::OP_DIV, true, false);break;
        }
    }

    void lex_comment(){
        switch(current_char)
        {
            case '\n': change_state(State::S_START, false); break;
        }
    }

    void lex_multiline(){
        switch(current_char)
        {
            case '#': change_state(State::S_MULTILINECOMMENT_EXIT, false); break;
        }
    }

    void lex_m_com_exit()
    {
        switch(current_char){
            case '/': change_state(State::S_START, false); break;
            default: change_state(State::S_MULTILINECOMMENT, false); break;
        }
    }

    void lex_op_and(){
        switch(current_char){
            case '&' : complete_token(TokenType::OP_LOGIC_AND, false); break;
            default: printf("lexer error at %d:%d : unexpected character '%c'\n", current_line, lexeme_start, '&');
                     running = false;
                     break;
        }
    }

    void lex_op_or(){
        switch(current_char){
            case '|' : complete_token(TokenType::OP_LOGIC_OR, false); break;
            default: printf("lexer error at %d:%d : unexpected character '%c'\n", current_line, lexeme_start, '|');
                     running = false;
                     break;
        }
    }

    void lex_op_dot(){
        switch(current_char)
        {
            case 'a' ... 'z': 
            case 'A' ... 'Z':
            case '_' :
                complete_token(TokenType::OP_FIELD_ACCESS, true); break;
            case '0' ... '9':
                change_state(State::S_LIT_FLOAT, true); break;
            default: printf("lexer error at %d:%d : unexpected character '%c'\n", current_line, current_column, current_char);
                     running = false;
                     break;

        }
    }

    void lex_op_high()
    {
        switch(current_char)
        {
            case '=': complete_token(TokenType::OP_HE, false, false); break;
            default: complete_token(TokenType::OP_H, true, false); break;
        }
    }

    void lex_op_low()
    {
        switch(current_char)
        {
            case '=': complete_token(TokenType::OP_LE, false, false); break;
            default: complete_token(TokenType::OP_L, true, false); break;
        }
    }

    void lex_op_equal()
    {
        switch(current_char)
        {
            case '=': complete_token(TokenType::OP_E, false, false); break;
            default: complete_token(TokenType::OP_ASSIGN, true, false); break;
        }
    }

    void lex_op_plus(){
        switch(current_char){
            case '+': complete_token(TokenType::OP_INCREMENT, false, false); break;
            default: complete_token(TokenType::OP_PLUS, true, false); break;
        }
    }

    void lex_op_minus(){
        switch(current_char){
            case '-': complete_token(TokenType::OP_DECREMENT, false, false); break;
            default: complete_token(TokenType::OP_MINUS, true, false); break;
        }
    }

    void lex_lit_int(){
        switch (current_char){
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '_' :
                    printf("lexer error at %d:%d : unexpected character '%c'\n", current_line, current_column, current_char);
                    running = false;
                    break;
            case '0' ... '9': add_to_lexeme(); break;
            case '.': 
                change_state(State::S_LIT_FLOAT);
                break;
            default: complete_token(TokenType::LIT_INT, true);
        }
        
    }

    void lex_lit_float(){
        switch(current_char){
            case 'a' ... 'd':
            case 'f' ... 'z':
            case 'A' ... 'D':
            case 'F' ... 'Z':
            case '_' :
                    printf("lexer error at %d:%d : unexpected character '%c'\n", current_line, current_column, current_char);
                    running = false;
                    break;
            case '0' ... '9': add_to_lexeme(); break;
            case 'E' :
            case 'e' : change_state(State::S_LIT_FLOAT_EXP_NEG); break;
            default: complete_token(TokenType::LIT_FLOAT, true);
        }
    }

    void lex_lit_float_exp_neg()
    {
        switch(current_char){
            case '-' : change_state(State::S_LIT_FLOAT_EXP); break;
            case '+' : change_state(State::S_LIT_FLOAT_EXP); break;
            case '0' ... '9' : change_state(State::S_LIT_FLOAT_EXP); break;
            default: printf("lexer error at %d:%d : unexpected character '%c', number or sign must follow exponent\n", current_line, current_column, current_char);
                    running = false;
                    break;
        }
    }

    void lex_lit_float_exp(){
        
        switch(current_char){
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '_' :
                    printf("lexer error at %d:%d : unexpected character '%c'\n", current_line, current_column, current_char);
                    running = false;
                    break;
            case '0' ... '9': add_to_lexeme(); break;
            default: complete_token(TokenType::LIT_FLOAT, true);
        }
    }

    void lex_lit_str()
    {
        switch(current_char){
            case '\n' : 
                    printf("lexer error at %d:%d : unterminated string, missing '\"'\n", current_line, current_column);
                    running = false;
                    break;
            case '\"' : complete_token(TokenType::LIT_STR, false); break;
            case '\\' : change_state(State::S_ESCAPE_CHAR, false); break;
            default: add_to_lexeme();
        }
    }

    void lex_escape_char(){
        switch(current_char){
            case 'n' : add_to_lexeme('\n'); break;
            case 't' : add_to_lexeme('\t'); break;
            default : add_to_lexeme(); break;
        }
        change_state(State::S_LIT_STR, false);
    }

    void lex_ident(){
        switch(current_char){
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '_' :
            case '0' ... '9': add_to_lexeme(); break;
            default: complete_token(TokenType::IDENT, true);
        }
    }

    void lex_start(){
        lexeme_start = current_column;
        switch(current_char){
            case 'a' ... 'z': 
            case 'A' ... 'Z':
            case '_' :
                change_state(State::S_IDENT);
                break;
            case '0' ... '9':
                change_state(State::S_LIT_INT);
                break;
            case ' ': break;
            case '\n' : current_line++; current_column = 0;break;
            case '.' :
                change_state(State::S_OP_DOT);
                break;
            case '\"': change_state(State::S_LIT_STR, false); break;
            case '+' : change_state(State::S_OP_PLUS, false); break;
            case '-' : change_state(State::S_OP_MINUS, false); break;
            case '>' : change_state(State::S_OP_HIGH, false); break;
            case '<' : change_state(State::S_OP_LOW, false); break;
            case '=' : change_state(State::S_OP_EQUAL, false); break;
            case '#' : change_state(State::S_COMMENT, false); break;
            case '/' : change_state(State::S_OP_SLASH, false); break;
            case '&' : change_state(State::S_OP_AND, false); break;
            case '|' : change_state(State::S_OP_OR, false); break;
            case '*' : complete_token(TokenType::OP_MULT, false); break;
            case '!' : complete_token(TokenType::OP_LOGIC_NOT, false); break;
            case '{' : complete_token(TokenType::OP_CB_OPEN, false); break;
            case '}' : complete_token(TokenType::OP_CB_CLOSE, false); break;
            case '(' : complete_token(TokenType::OP_PAREN_OPEN, false); break;
            case ')' : complete_token(TokenType::OP_PAREN_CLOSE, false); break;
            case '[' : complete_token(TokenType::OP_SB_OPEN, false); break;
            case ']' : complete_token(TokenType::OP_SB_CLOSE, false); break;
            case ',' : complete_token(TokenType::OP_COMMA_SEP, false); break;
            case ';' : complete_token(TokenType::OP_SEMICOLON_SEP, false); break;
            default: 
                printf("lexer error at %d:%d : unrecognized character '%c'\n", current_line, lexeme_start, current_char);
                running = false;
        }
    }

};

int main(int argc, char** argv)
{
    if(argc < 2 || argc > 3){
        std::cout << "Usage: lexer <filename>" <<std::endl;
    }else{
        Lexer lexer;
        lexer.input = get_file_contents(argv[1]);
        lexer.lex_all();

    }
}