#include "symbol_table.hh"


using namespace symbol_table;


Symbol::Symbol(Value value, Tokens::TokenType type)
: value(value), type(type)
{
    
}


std::ostream& operator<<(std::ostream& stream, const symbol_table::Symbol& symbol)
{
    stream << '{' << symbol.type << ' ' << symbol.value << '}';
}

