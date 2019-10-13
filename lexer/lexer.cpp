#include <iostream>
#include <string>
#include <list>


enum TokenType{
    IDENT, LIT_INT, OP_PLUS
};

struct Token{
    TokenType type;
    std::string value;
    int line_no;

    Token(){}
    Token(TokenType _type, std::string _value, int _line_no) : type(_type), value(_value), line_no(_line_no) {}
};

enum State{
    S_START, S_IDENT, S_OP_PLUS, S_LIT_INT
};

class Lexer{
public:
    std::string input;
    int offset;

    char current_char;
    State current_state;
    std::string current_lexeme;
    int current_line = 0;

    //std::list<Token> tokens;

    void print_tokens(){
       // for(Token t : tokens){
        //    std::cout << t.type << "  " << t.value << std::endl;
        //}
    }

    void begin_token(State new_state){
        current_state = new_state;
        current_lexeme += current_char;
    }

    void complete_token(TokenType type, bool decrement){
        std::cout<<"wow" << std::endl;
        Token toAdd;
        toAdd.type = type;
        toAdd.value = current_lexeme;
        toAdd.line_no = current_line;
        current_lexeme = "";

        //tokens.push_back(toAdd);

        if(decrement) offset--;
    }

    void add_to_lexeme(){
        current_lexeme += current_char;
    }

    void lex_all(){
        std::cout<<"wow" << std::endl;
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
            
        }
    }

    void lex_lit_int(){
        switch (current_char){
            case '0' ... '9': add_to_lexeme(); break;
            default: complete_token(TokenType::LIT_INT, true);
        }
        
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
                add_to_lexeme();
                begin_token(State::S_IDENT);
                break;
            case '0' ... '9':
                add_to_lexeme();
                begin_token(State::S_LIT_INT);
            case ' ': break;
            case '\n' : current_line++; break;
            case '+' : complete_token(TokenType::OP_PLUS, false); break; 

        }
    }

};

int main()
{
    std::cout << "test jeez" << std::endl;
    std::string code_file = "asbasd + 12315";
    std::cout << "code_file" << std::endl;
    Lexer lexer;
    lexer.input = code_file;
    lexer.lex_all();
    

}