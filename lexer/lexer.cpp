#include <iostream>
#include <string>
#include <vector>


enum TokenType{
    IDENT, LIT_INT, LIT_FLOAT, LIT_STR, OP_PLUS, OP_INCREMENT, OP_MINUS, OP_DECREMENT, OP_L, OP_LE, OP_H, OP_HE, OP_ASSIGN, OP_E
};

const std::string TokenNames[] = {"IDENT", "LIT_INT", "LIT_FLOAT", "LIT_STR", "OP_PLUS", "OP_INCREMENT", "OP_MINUS", "OP_DECREMENT", "OP_L", "OP_LE", "OP_H", "OP_HE", "OP_ASSIGN", "OP_E"};

struct Token{
    TokenType type;
    std::string value;
    int line_no;

    Token(){}
    Token(TokenType _type, std::string _value, int _line_no) : type(_type), value(_value), line_no(_line_no) {}
};

enum State{
    S_START, S_IDENT, S_LIT_INT, S_LIT_FLOAT, S_LIT_FLOAT_EXP_NEG, S_LIT_FLOAT_EXP, S_LIT_STR, S_ESCAPE_CHAR, S_OP_PLUS, S_OP_MINUS, S_OP_LOW, S_OP_HIGH, S_OP_EQUAL
};


class Lexer{
public:
    std::string input;
    int offset = 0;

    char current_char;
    State current_state = State::S_START;
    std::string current_lexeme;
    int current_line = 0;

    std::vector<Token> tokens;

    void print_tokens(){
        for(Token t : tokens){
            std::cout << TokenNames[t.type] << "  " << t.value << std::endl;
        }
    }

    void begin_token(State new_state)
    {
        current_state = new_state;
        current_lexeme += current_char;
    }

    void begin_token(State new_state, bool add_to_lexeme){
        current_state = new_state;
        if(add_to_lexeme)
        {
            current_lexeme += current_char;
        }
    }

    void complete_token(TokenType type, bool decrement){
        Token toAdd;
        toAdd.type = type;
        toAdd.value =  current_lexeme;
        toAdd.line_no = current_line;
        current_state = State::S_START;
        current_lexeme = "";

        tokens.push_back(toAdd);

        if(decrement) offset--;
    }

    void complete_token(TokenType type, bool decrement, bool with_value){
        Token toAdd;
        toAdd.type = type;
        toAdd.value = with_value ? current_lexeme : "";
        toAdd.line_no = current_line;
        current_state = State::S_START;
        current_lexeme = "";

        tokens.push_back(toAdd);

        if(decrement) offset--;
    }

    void add_to_lexeme(){
        current_lexeme += current_char;
    }

    void add_to_lexeme(char c){
        current_lexeme += c;
    }

    void lex_all(){
        std::cout<< input << std::endl;
        while(offset < input.size()){
            current_char = input[offset];
            lex();
            offset++;
        }

        print_tokens();
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
            case '0' ... '9': add_to_lexeme(); break;
            case '.': 
                begin_token(State::S_LIT_FLOAT);
                break;
            default: complete_token(TokenType::LIT_INT, true);
        }
        
    }

    void lex_lit_float(){
        switch(current_char){
            case '0' ... '9': add_to_lexeme(); break;
            case 'e' : begin_token(State::S_LIT_FLOAT_EXP_NEG); break;
            default: complete_token(TokenType::LIT_FLOAT, true);
        }
    }

    void lex_lit_float_exp_neg()
    {
        switch(current_char){
            case '-' : begin_token(State::S_LIT_FLOAT_EXP); break;
            default: begin_token(State::S_LIT_FLOAT_EXP); break;
        }
    }

    void lex_lit_float_exp(){
        
        switch(current_char){
            case '0' ... '9': add_to_lexeme(); break;
            case 'e' : begin_token(State::S_LIT_FLOAT_EXP_NEG); break;
            default: complete_token(TokenType::LIT_FLOAT, true);
        }
    }

    void lex_lit_str()
    {
        switch(current_char){
            case '\n' : //TODO: error
            case '\"' : complete_token(TokenType::LIT_STR, false); break;
            case '\\' : begin_token(State::S_ESCAPE_CHAR, false); break;
            default: add_to_lexeme();
        }
    }

    void lex_escape_char(){
        switch(current_char){
            case 'n' : add_to_lexeme('\n'); break;
            case 't' : add_to_lexeme('\t'); break;
            default : add_to_lexeme(); break;
        }
        begin_token(State::S_LIT_STR, false);
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
        switch(current_char){
            case 'a' ... 'z': 
            case 'A' ... 'Z':
            case '_' :
                begin_token(State::S_IDENT);
                break;
            case '0' ... '9':
                begin_token(State::S_LIT_INT);
                break;
            case ' ': break;
            case '\n' : current_line++; break;
            case '+' : begin_token(State::S_OP_PLUS, false); break;
            case '-' : begin_token(State::S_OP_MINUS, false); break;
            case '>' : begin_token(State::S_OP_HIGH, false); break;
            case '<' : begin_token(State::S_OP_LOW, false); break;
            case '=' : begin_token(State::S_OP_EQUAL, false); break; 
            case '.' :
                begin_token(State::S_LIT_FLOAT);
                break;
            case '\"': begin_token(State::S_LIT_STR, false);

        }
    }

};

int main()
{
    std::string code_file = "cop = 5>3>=bzz<=c<2==a++";
    Lexer lexer;
    lexer.input = code_file;
    lexer.lex_all();
    

}