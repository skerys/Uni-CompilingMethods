#pragma once

#include "ast_stmt.h"
#include "../code_gen/label.h"



enum AccesibilityName{
    PUBLIC, PRIVATE
};



class Decl : public Node{
public:
    Token* name;
    Decl(Token* _name) : name(_name){}
};

class Program : public Node{
    std::vector<Decl*> decls;

    public:
    Label* mainLabel = nullptr;
    Program(std::vector<Decl*> _decls) : decls(_decls)
    {
        printf("decls size : %d", _decls.size());
        add_children(decls);
    }

    void addDecl(Decl* d){
        decls.push_back(d);
    }

    void print_node(){
        print_text("Program:" );
        indentLevel++;
        for(int i = 0; i < decls.size(); i++){
            print_text("decl["+stringulate(i)+"]:", false);
            decls[i]->print_node();
        }
        indentLevel--;
    }

    void resolve_names(Scope* scope){
        for(auto&& fn : decls){
            scope->add(fn->name, fn);
        }
        for(auto&& fn : decls){
            fn->resolve_names(scope);
        }
        if(mainLabel == nullptr){
            print_error(lastToken, "no 'main' found\n");
        }
    }

    void gen_code(CodeWriter w){
        printf("yyet");
        w.write(InstrName::I_CALL_BEGIN);
        std::vector<int> zero;
        zero.push_back(0);
        w.write(InstrName::I_CALL, mainLabel, zero);
        w.write(InstrName::I_EXIT);
        for(auto&& decl : decls){
            decl->gen_code(w);
        }
    }

    Type* check_types(){
        for(auto&& fn : decls){
            fn->check_types();
        }
        return nullptr;
    }
};

//ClassBodyStmt, ClassBody, ClassDecl

class FnDecl : public Decl{
public:
    Type* returnType;
    std::vector<Param*> params;
    StmtBlock* body;
    Scope* mScope;
    Label* startLabel;
    int numLocals;
    FnDecl(Type* _returnType,
           Token* _name, std::vector<Param*> _params, StmtBlock* _body) : returnType(_returnType), Decl(_name), params(_params), body(_body) 
    {
        add_children(params);
        add_children(returnType);
        add_children(body);
        startLabel = new Label();
    }

    void print_node(){
        print_text("FunctionDef:");
        indentLevel++;
        print_text("returnType: " + returnType->get_type_name());
        print_text("name: " + std::get<std::string>(name->value));
        for(int i = 0; i < params.size(); i++){
            print_text("param[" + stringulate(i) +"]:", false);
            params[i]->print_node();
        }
        print_text("body: ", false);
        body->print_node();
        indentLevel--;
    }
    void resolve_names(Scope* scope){
        if(std::get<std::string>(name->value) == "main"){
            bool mainFollowsSig = false;
            Program* program = find_ancestor<Program>();
            program->mainLabel = startLabel;
            
            auto primitiveType = dynamic_cast<PrimitiveType*>(returnType);
            if(primitiveType){
                if(primitiveType->type == PrimitiveTypeName::INT){
                    if(params.size() == 1){
                        auto primitiveParamType = dynamic_cast<PrimitiveType*>(params[0]->type);
                        if(primitiveParamType){
                            if(primitiveParamType->type == PrimitiveTypeName::STRING){
                                mainFollowsSig = true;
                            }
                        }
                    }
                }
            }
            if(!mainFollowsSig){
                print_error(name, "'main' does not follow the standard signature 'func int main(string s)'\n");
            }   
        }

        stackSlotIndex = 0;
        mScope = new Scope(scope);
        for(auto&& p : params){
            p->resolve_names(mScope);
        }
        body->resolve_names(mScope);
        numLocals = stackSlotIndex;
    }

    void gen_code(CodeWriter w){
        printf("yyet");
        
        w.place_label(startLabel);
        printf("yyet");
        if(numLocals > 0){
            std::vector<int> operands;
            operands.push_back(numLocals);
            w.write(InstrName::I_ALLOC, operands);
        }
        printf("yyet");
        body->gen_code(w);
        printf("yyet");
        w.write(InstrName::I_RET);
    }

    Type* get_type()
    {
        return returnType;
    }

    Type* check_types(){
        for(auto&& p : params){
            p->check_types();
        }
        body->check_types();
        return nullptr;
    }

    Label* get_start_label()
    {
        return startLabel;
    }
};

Type* FnCallExpr::check_types()
    {
        std::vector<Type*> argTypes;
        for(auto&& a : callArgs){
            argTypes.push_back(a->check_types());
        }
        if(targetNode == nullptr){
            return nullptr;
        }
        FnDecl* targetDecl;
        targetDecl = dynamic_cast<FnDecl*>(targetNode);
        if(!targetDecl){
            //printf("error: call target '%s' is not a function at %d:%d\n", std::get<std::string>(toCall->value).c_str(), toCall->line_no, toCall->column_no);
            print_error(toCall, "call target '" + std::get<std::string>(toCall->value) + "' is not a function\n");
            return nullptr;
        }

        std::vector<Type*> paramTypes;
        for(auto&& p : targetDecl->params){
            paramTypes.push_back(p->get_type());
        }

        if(argTypes.size() != paramTypes.size()){
            printf("%d:%d error: invalid argument count, expected %d, got %d\n", toCall->line_no, toCall->column_no, paramTypes.size(), argTypes.size());

            print_error(toCall, "invalid argument count, expected " + stringulate(paramTypes.size()) + " got " + stringulate(argTypes.size()) + "\n");
        }

        int paramCount = std::min(argTypes.size(), paramTypes.size());

        for(int i = 0; i < paramCount; i++){
            unify_types(paramTypes[i], argTypes[i], toCall);
        }
        return targetNode->get_type();
    }

Type* VarExpr::check_types()
{
    if(target){
        if(dynamic_cast<FnDecl*>(target)){
            return new PrimitiveType(PrimitiveTypeName::FUNCTION);
        }
        else{
            return target->get_type();
        }
    }
    else return nullptr;

}


