#include "symbol_table.hh"
#include "token.hh"
#include "utils.hh"
#include "unordered_map"

#include <iostream>


using namespace symbol_table;


// initialize Symbol Table
std::unordered_map<std::string, Symbol*> SymbolTable::table = std::unordered_map<std::string, Symbol*>();

ScopeStack SymbolTable::scopes = ScopeStack();


void SymbolTable::assign(std::string* identifier, Symbol* symbol)
{
    // check if token was not declared
    if (table.find(*identifier) == table.end())
    {
        // TODO implement error handling
        std::cerr << "\"" << *identifier << "\" was not declared" << std::endl;
        exit(1);
    }

    // TODO implement type checking

    // dereference table since it's allocated on the heap
    table[*identifier] = symbol;
}


void SymbolTable::declare(std::string* identifier, Symbol* symbol)
{
    // check if token was already declared
    if (table.find(*identifier) != table.end())
    {
        // TODO implement error handling
        // TODO implement scope specific checking
        std::cerr << "\"" << *identifier << "\" was already declared" << std::endl;
        exit(1);
    }

    table[*identifier] = symbol;
}


// gets a Symbol given its name
Symbol* SymbolTable::get(std::string* identifier)
{
    std::unordered_map<std::string, Symbol*>::iterator iterator;
    iterator = table.find(*identifier);

    // if Symbold does not exist
    if (iterator == table.end())
    {
        std::cerr << "\"" << *identifier << "\" was not declared" << std::endl;
        exit(1);
    }

    // return the symbol
    return iterator->second;
}


// pushes a new Scope to the SymbolTable
void SymbolTable::pushScope()
{
    // the new Scope should point to where it begins in the SymbolTable
    scopes.push(table.size());
}


// pops the last Scope form the SymbolTable
void SymbolTable::popScope()
{
    // first pop the scope
    Scope* scope = scopes.pop();
    // TODO notify the table that the scope has been popped
}

