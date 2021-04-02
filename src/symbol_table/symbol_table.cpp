#include "symbol_table.hh"
#include "token.hh"
#include "utils.hh"
#include "unordered_map"

#include <iostream>


using namespace symbol_table;


// initialize Symbol Table
std::unordered_map<Tokens::Token*, Value>* SymbolTable::table = 
    new std::unordered_map<Tokens::Token*, Value>();

ScopeStack* SymbolTable::scopes = new ScopeStack();


void SymbolTable::assign(Tokens::Token* token, Value value)
{
    // check if token was not declared
    if (table->find(token) == table->end())
    {
        // TODO implement error handling
        std::cerr << *token << " was not declared" << std::endl;
        exit(1);
    }

    // dereference table since it's allocated on the heap
    (*table)[token] = value;
}


void SymbolTable::declare(Tokens::Token* token)
{
    // check if token was already declared
    if (table->find(token) != table->end())
    {
        // TODO implement error handling
        // TODO implement scope specific checking
        std::cerr << *token << " was already declared" << std::endl;
        exit(1);
    }

    (*table)[token] = 0;
}


Value SymbolTable::get(Tokens::Token* token)
{
    std::unordered_map<Tokens::Token*, Value>::iterator iterator;
    iterator = table->find(token);

    if (iterator == table->end())
    {
        std::cerr << token << " was not declared" << std::endl;
        exit(1);
    }

    return iterator->second;
}

