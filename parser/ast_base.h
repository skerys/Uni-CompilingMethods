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

class Param : Node{
    std::string name;
    std::string type;
};



