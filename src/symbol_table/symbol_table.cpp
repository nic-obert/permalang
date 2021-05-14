#include "pch.hh"

#include "symbol_table.hh"


using namespace symbol_table;


Scope* SymbolTable::scopeStack = nullptr;

Scope* SymbolTable::globalScope = nullptr;


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
    if (globalScope->local.find(*identifier) != globalScope->local.end())
    {
        globalScope->local[*identifier] = symbol;
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
    // update scope size
    scopeStack->localSymbolsSize += Tokens::typeSize(symbol->type);
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
    iterator = globalScope->local.find(*identifier);

    if (iterator != globalScope->local.find(*identifier))
    {
        return iterator->second;
    }
    
    // if symbol has not been found, it wasn't declared in 
    // any reachable scope, thus throw exception
    std::cerr << "\"" << *identifier << "\" was not declared" << std::endl;
    exit(1);
}


const Scope* SymbolTable::getScope()
{
    return scopeStack;
}


// pushes a new Scope to the ScopeStack
void SymbolTable::pushScope(bool inherits)
{
    Scope* scope;

    if (inherits)
    {
        // create a new scope that inherits from the previous scope
        scope = new Scope(scopeStack->local, scopeStack->outer);
    }
    else
    {
        // create a completely new, independent Scope
        scope = new Scope();
    }

    // push the new Scope to the stack
    scope->prev = scopeStack;
    scopeStack = scope;
}


// pops the last Scope form the ScopeStack
void SymbolTable::popScope()
{   
    // pop scope from the stack
    Scope* tmpScope = scopeStack;
    scopeStack = scopeStack->prev;

    // delete popped scope
    delete tmpScope;
}


void SymbolTable::clear()
{
    // pop the last scope and unset globalScope
    popScope();
    globalScope = nullptr;
}


void SymbolTable::init()
{
    scopeStack = new Scope();
    globalScope = scopeStack;
}

