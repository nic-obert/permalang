#include "pch.hh"


using namespace symbol_table;


Symbol::Symbol(Value value, Tokens::TokenType type)
: value(value), type(type)
{
    
}

