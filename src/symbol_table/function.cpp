#include "symbol_table.hh"


using namespace symbol_table;


Parameter::Parameter(Symbol&& symbol, std::string&& name)
: symbol(std::move(symbol)), name(std::move(name))
{
    
}


Function::Function()
{

}


Function::Function(Tokens::TokenType returnType, syntax_tree::SyntaxTree&& body, std::vector<Parameter>&& parameters)
: returnType(returnType), body(std::move(body)), parameters(std::move(parameters))
{

}


