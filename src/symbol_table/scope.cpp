#include "pch.hh"

#include "symbol_table.hh"


using namespace symbol_table;


Scope::Scope()
: localSymbolsSize(0)
{   
    // just create a new local scope
    // without inheriting from outer scopes
    local = Table();
}


Scope::Scope(const Table& _local, const std::optional<Table>& _outer)
: localSymbolsSize(0)
{
    // create a new local scope
    local = Table();

    // copy previous local scope first
    // local symbols have precedence over outer ones
    outer = Table(_local);
    
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

