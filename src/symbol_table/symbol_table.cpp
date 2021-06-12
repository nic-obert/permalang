#include "symbol_table.hh"
#include "errors.hh"


using namespace symbol_table;


Scope* SymbolTable::scopeStack = nullptr;

Scope* SymbolTable::globalScope = nullptr;

size_t SymbolTable::stackPointer = 0;


void SymbolTable::assign(std::string* identifier, Symbol* symbol)
{   
    // size compatibility is guaranteed by the tree building phase

    // search in local scope first    
    if (scopeStack->local.find(*identifier) != scopeStack->local.end())
    {
        Symbol* oldSymbol = scopeStack->local[*identifier];
        scopeStack->local[*identifier] = symbol;
        delete oldSymbol;
        return;
    }

    if (scopeStack->outer.has_value()
        && scopeStack->outer.value().find(*identifier) != scopeStack->outer.value().end())
    {
        Symbol* oldSymbol = scopeStack->outer.value()[*identifier];
        scopeStack->outer.value()[*identifier] = symbol;
        delete oldSymbol;
        return;
    }

    if (globalScope->local.find(*identifier) != globalScope->local.end())
    {
        Symbol* oldSymbol = globalScope->local[*identifier];
        globalScope->local[*identifier] = symbol;
        delete oldSymbol;
        return;
    }

    // if symbol has not been found, it wasn't declared in 
    // any reachable scope, thus throw exception
    errors::UndefinedSymbolError(*identifier);
}


void SymbolTable::declare(std::string* identifier, Symbol* symbol)
{
    // check if token was already declared in local scope
    if (scopeStack->local.find(*identifier) != scopeStack->local.end())
    {
        errors::SymbolRedeclarationError(*identifier, *symbol);
    }

    scopeStack->local[*identifier] = symbol;

    symbol->stackPosition = stackPointer;
    
    size_t symbolSize = Tokens::typeSize(symbol->type);
    stackPointer += symbolSize;
    scopeStack->localSymbolsSize += symbolSize;  
}


Symbol* SymbolTable::get(std::string* identifier)
{
    // check in local scope first
    Table::const_iterator iterator = scopeStack->local.find(*identifier);

    if (iterator != scopeStack->local.cend())
    {
        return iterator->second;
    }

    if (scopeStack->outer.has_value())
    {
        iterator = scopeStack->outer.value().find(*identifier);

        if (iterator != scopeStack->outer.value().cend())
        {
            return iterator->second;
        }
    }
    
    iterator = globalScope->local.find(*identifier);

    if (iterator != globalScope->local.find(*identifier))
    {
        return iterator->second;
    }
    
    // if symbol has not been found, it wasn't declared in 
    // any reachable scope, thus throw exception
    errors::UndefinedSymbolError(*identifier);
    // this line never gets reached
    return nullptr;
}


const Scope* SymbolTable::getScope()
{
    return scopeStack;
}


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

    scope->prev = scopeStack;
    scopeStack = scope;
}


void SymbolTable::popScope()
{   
    Scope* tmpScope = scopeStack;
    scopeStack = scopeStack->prev;

    delete tmpScope;
}


void SymbolTable::clear()
{
    popScope();
    globalScope = nullptr;
}


void SymbolTable::init()
{
    scopeStack = new Scope();
    globalScope = scopeStack;
}


size_t SymbolTable::getStackPointer()
{
    return stackPointer;
}

