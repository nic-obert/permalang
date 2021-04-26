#include "symbol_table.hh"
#include "token.hh"
#include "utils.hh"
#include "unordered_map"

#include <iostream>


using namespace symbol_table;


// initialize SymbolTable
Table SymbolTable::globalScope = Table();

Scope* SymbolTable::scopeStack = new Scope();


void SymbolTable::assign(std::string* identifier, Symbol* symbol)
{   
    // TODO implement type checking
    // search in local scope first    
    if (scopeStack->local.find(*identifier) != scopeStack->local.end())
    {
        // dereference table since identifier is allocated on the heap
        scopeStack->local[*identifier] = symbol;
        return;
    }

    // check then in outer scope
    if (scopeStack->outer.has_value()
        && scopeStack->outer.value().find(*identifier) != scopeStack->outer.value().end())
    {
        scopeStack->outer.value()[*identifier] = symbol;
        return;
    }

    // lastly check in global scope
    if (globalScope.find(*identifier) != globalScope.end())
    {
        globalScope[*identifier] = symbol;
        return;
    }

    // if symbol has not been found, it wasn't declared in 
    // any reachable scope, thus throw exception
    // TODO implement error handling
    std::cerr << "\"" << *identifier << "\" was not declared" << std::endl;
    exit(1);
}


void SymbolTable::declare(std::string* identifier, Symbol* symbol)
{
    // check if token was already declared in local scope
    if (scopeStack->local.find(*identifier) != scopeStack->local.end())
    {
        // TODO implement error handling
        // TODO implement scope specific checking
        std::cerr << "\"" << *identifier << "\" was already declared" << std::endl;
        exit(1);
    }

    // insert symbol 
    scopeStack->local[*identifier] = symbol;
}


// gets a Symbol given its name
Symbol* SymbolTable::get(std::string* identifier)
{
    // check in local scope first
    Table::const_iterator iterator = scopeStack->local.find(*identifier);

    if (iterator != scopeStack->local.cend())
    {
        return iterator->second;
    }

    // then in outer scope
    if (scopeStack->outer.has_value())
    {
        iterator = scopeStack->outer.value().find(*identifier);

        if (iterator != scopeStack->outer.value().cend())
        {
            return iterator->second;
        }
    }
    

    // lastly in global scope
    iterator = globalScope.find(*identifier);

    if (iterator != globalScope.find(*identifier))
    {
        return iterator->second;
    }
    
    // if symbol has not been found, it wasn't declared in 
    // any reachable scope, thus throw exception
    std::cerr << "\"" << *identifier << "\" was not declared" << std::endl;
    exit(1);
}

