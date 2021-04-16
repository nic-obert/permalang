#include "symbol_table.hh"


using namespace symbol_table;


Scope::Scope(unsigned int scope, size_t index)
: scope(scope), index(index)
{}


ScopeStack::ScopeStack()
{
    last = new Scope(0, 0);
}


// pushes a new Scope to the ScopeStack
void ScopeStack::push(size_t index)
{
    // create a new Scope with a scope depth of previous scope's incremented by 1
    Scope* scope = new Scope(last->scope + 1, index);

    // push new Scope to the stack
    // last is always defined
    scope->prev = last;
    last = scope;
}


// pops the last Scope form the ScopeStack
Scope* ScopeStack::pop()
{
    Scope* tmpScope = last;
    last = last->prev;
    delete tmpScope;
}
