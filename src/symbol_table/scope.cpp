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
    local = Table();

    // copy previous local scope first
    // local symbols have precedence over outer ones
    outer = Table(_local);
    
    // check if the given outer scope exists
    if (_outer.has_value())
    {
        outer.value().insert(_outer.value().begin(), _outer.value().end());
    }
}


Scope::~Scope()
{   
    // delete only local symbols since they are the only one belonging to the local scope
    for (auto it = local.begin(); it != local.end(); ++it)
    {
        delete it->second;
    }
}

