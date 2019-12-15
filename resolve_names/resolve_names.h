#pragma once

#include "../lexer/lexer.h"
#include "../parser/ast_decl.h"

class Scope{
public:
    Scope* parentScope;
    std::map<std::string, Node*> members;
    Scope(Scope* _parentScope) : parentScope(_parentScope) {} 

    void add(Token* token, Node* node){
        auto name = std::get<std::string>(token->value);
        if(members.find(name) == members.end()){
            //not found
            members[name] = node;
        }else{
            //found
            printf("duplicate variable: %s at %d:%d", name, token->line_no, token->column_no);
        }
    }

    Node* resolve_name(Token* token){
        auto name = std::get<std::string>(token->value);
        if(members.find(name) != members.end()){
            //found
            return members[name];
        }

        if(parentScope != nullptr){
            return parentScope->resolve_name(token);
        }

        printf("undeclared variable: %s at %d:%d", name, token->line_no, token->column_no);
    }
};

