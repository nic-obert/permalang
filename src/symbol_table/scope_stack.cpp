#include "pch.hh"

#include "symbol_table.hh"


using namespace symbol_table;


Scope::Scope()
{   
    // just create a new local scope
    // without inheriting from outer scopes
    local = Table();
}


Scope::Scope(Table& _local, std::optional<Table>& _outer)
{
    // create a new local scope
    local = Table();

    outer = Table();
    
    // insert the previous local scope first
    // local symbols have precedence over outer ones
    outer.value().insert(_local.begin(), _local.end());

    // check if the given outer scope exists
    if (_outer.has_value())
    {
        // now insert the previous outer scope
        outer.value().insert(_outer.value().begin(), _outer.value().end());
    }
}


Scope::~Scope()
{   
    // delete iteratively all symbols in local scope
    for (auto it = local.begin(); it != local.end(); ++it)
    {
        delete it->second;
    }
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
