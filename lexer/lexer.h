#pragma once

#include <string>
#include <map>
#include <variant>
#include <iostream>
#include <vector>

enum TokenType{
    IDENT, LIT_INT, LIT_FLOAT, LIT_STR, OP_PLUS, OP_INCREMENT, OP_MINUS, OP_DECREMENT, OP_L, OP_LE, OP_H, OP_HE, OP_ASSIGN, OP_E,
    OP_PAREN_OPEN, OP_PAREN_CLOSE, OP_CB_OPEN, OP_CB_CLOSE, OP_SB_OPEN, OP_SB_CLOSE, OP_MULT, OP_DIV, OP_LOGIC_AND, OP_LOGIC_OR, OP_LOGIC_NOT, OP_FIELD_ACCESS, OP_COMMA_SEP, OP_SEMICOLON_SEP, OP_NEWLINE_SEP,
    KW_IF, KW_ELIF, KW_ELSE, KW_WHILE, KW_FOR, KW_RETURN, KW_BREAK, KW_NEXT, KW_READ, KW_WRITE, KW_CLASS, KW_PRIVATE, KW_PUBLIC, KW_FUNC, KW_INT, KW_FLOAT, KW_BOOL, KW_STRING, KW_VOID, KW_TRUE, KW_FALSE, EOF_
};

const std::string TokenNames[] = {"IDENT", "LIT_INT", "LIT_FLOAT", "LIT_STR", "OP_PLUS", "OP_INCREMENT", "OP_MINUS", "OP_DECREMENT", "OP_L", "OP_LE", "OP_H", "OP_HE", "OP_ASSIGN", "OP_E",
                                  "OP_PAREN_OPEN", "OP_PAREN_CLOSE", "OP_CB_OPEN", "OP_CB_CLOSE", "OP_SB_OPEN", "OP_SB_CLOSE", "OP_MULT", "OP_DIV", "OP_LOGIC_AND", "OP_LOGIC_OR", "OP_LOGIC_NOT",
                                  "OP_FIELD_ACCESS", "OP_COMMA_SEP", "OP_SEMICOLON_SEP", "OP_NEWLINE_SEP",
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

struct TokenValueVisitor
{
    void operator()(int i) const { 
        std::cout << i;
    }
    void operator()(float f) const { 
        std::cout << f; 
    }
    void operator()(const std::string& s) const { 
        std::cout << s;
    }
};

struct Token{
    int id;
    TokenType type;
    std::variant<int, float, std::string> value;
    int line_no;
    int column_no;

    Token(){}
    Token(TokenType _type, std::string _value, int _line_no) : type(_type), value(_value), line_no(_line_no) {}
};

std::vector<Token> lex_file(const char* path, bool printTokens);

enum State{
    S_START, S_IDENT, S_LIT_INT, S_LIT_FLOAT, S_LIT_FLOAT_EXP_NEG, S_LIT_FLOAT_EXP, S_LIT_STR, S_ESCAPE_CHAR,
    S_OP_PLUS, S_OP_MINUS, S_OP_LOW, S_OP_HIGH, S_OP_EQUAL, S_OP_OR, S_OP_AND, S_OP_DOT, S_OP_SLASH, S_COMMENT, S_MULTILINECOMMENT, S_MULTILINECOMMENT_EXIT
};

