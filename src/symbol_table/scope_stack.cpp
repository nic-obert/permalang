#include "symbol_table.hh"


using namespace symbol_table;


Scope::Scope(unsigned int scope, unsigned int index)
: scope(scope), index(index)
{}


ScopeStack::ScopeStack()
{
    last = new Scope(0, 0);
}


void ScopeStack::push(Scope* scope)
{
    // last is always defined
    scope->prev = last;
    last = scope;
}


Scope* ScopeStack::pop()
{
    Scope* tmpScope = last;
    last = last->prev;
    delete tmpScope;
}
