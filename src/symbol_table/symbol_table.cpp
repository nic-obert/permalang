#include "symbol_table.hh"
#include "token.hh"
#include "utils.hh"
#include "unordered_map"

#include <iostream>


using namespace symbol_table;


// initialize Symbol Table
std::unordered_map<std::string, Symbol*>* SymbolTable::table = 
    new std::unordered_map<std::string, Symbol*>();

ScopeStack* SymbolTable::scopes = new ScopeStack();


void SymbolTable::assign(std::string* identifier, Symbol* symbol)
{
    // check if token was not declared
    if (table->find(*identifier) == table->end())
    {
        // TODO implement error handling
        std::cerr << "\"" << *identifier << "\" was not declared" << std::endl;
        exit(1);
    }

    // TODO implement type checking

    // dereference table since it's allocated on the heap
    (*table)[*identifier] = symbol;
}


void SymbolTable::declare(std::string* identifier, Symbol* symbol)
{
    // check if token was already declared
    if (table->find(*identifier) != table->end())
    {
        // TODO implement error handling
        // TODO implement scope specific checking
        std::cerr << "\"" << *identifier << "\" was already declared" << std::endl;
        exit(1);
    }

    (*table)[*identifier] = symbol;
}


Symbol* SymbolTable::get(std::string* identifier)
{
    std::unordered_map<std::string, Symbol*>::iterator iterator;
    iterator = table->find(*identifier);

    if (iterator == table->end())
    {
        std::cerr << "\"" << *identifier << "\" was not declared" << std::endl;
        exit(1);
    }

    return iterator->second;
}

