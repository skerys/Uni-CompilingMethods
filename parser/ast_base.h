#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

class Node{
    virtual void print_node(){
        std::cout << "print node not implemented for type " << typeid(this).name() << std::endl;
    }
};

class Param : public Node{
    std::string name;
    std::string type;
public:
    Param(std::string _type, std::string _name) : type(_type), name(_name) {} 

};



