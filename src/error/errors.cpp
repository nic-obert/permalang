#include "errors.hh"


using namespace errors;


void errors::GenericError(std::string&& message)
{
    std::cerr << "[Error] " << message << std::endl;
    exit(EXIT_FAILURE);
}


void errors::SyntaxError(std::string&& message)
{
    std::cerr << "[Syntax Error] " << message << std::endl;
    exit(EXIT_FAILURE);
}


void errors::TypeError(Tokens::Token& caller, Tokens::TokenType expected, const Tokens::Token& provided, const char* side)
{
    std::cerr << "[Type Error] Expected type "
        << expected << " to the " << side << " of " << caller
        << ", but " << provided << " was provided" << std::endl;
    exit(EXIT_FAILURE);
}


void errors::TypeError(Tokens::Token& caller, OpCodes expected, const Tokens::Token& provided, const char* side)
{
    std::cerr << "[Type Error] Expected type "
        << expected << " to the " << side << " of " << caller
        << ", but " << provided << " was provided" << std::endl;
    exit(EXIT_FAILURE);
}


void errors::SymbolRedeclarationError(const symbol_table::Symbol& symbol)
{
    std::cerr << "[Symbol Redeclaration Error] Symbol " << symbol
        << " has been redeclared in local scope" << std::endl;
    exit(EXIT_FAILURE);
}


void errors::UndefinedSymbolError(std::string&& name)
{
    std::cerr << "[Undefined Symbol Error] Symbol name " << name
        << " hasn't been declared in any reachable scope" << std::endl;
    exit(EXIT_FAILURE);
}


void errors::InvalidCharacterError(std::string&& line, char character)
{
    std::cerr << "[Invalid Character Error] Invalid character '" << character
        << "' in line:\n" << line << std::endl;
    exit(EXIT_FAILURE);
}


void errors::ExpectedTokenError(Tokens::Token& caller, Tokens::TokenType expected, const char* side)
{
    std::cerr << "[Expected Token Error] Expected type " << expected
        << " to the " << side << " of " << caller << std::endl;
    exit(EXIT_FAILURE);
}


void errors::ExpectedTokenError(Tokens::Token& caller, OpCodes expected, const char* side)
{
    std::cerr << "[Expected Token Error] Expected type " << expected
        << " to the " << side << " of " << caller << std::endl;
    exit(EXIT_FAILURE);
}

