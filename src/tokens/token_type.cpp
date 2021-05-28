#include "token.hh"


using namespace Tokens;


// lokup table for TokenType string representation
static const char* const tokenTypeRepr[] =
{
    "NONE",
    "TEXT",
    "INT",
    "FLOAT",
    "STRING",
    "KEYWORD",
    "BOOL",
    "SCOPE",
    "PARENTHESIS",
    "ENDS",
    "DOUBLE",
    "LONG",
    "FUNCTION"
};


std::ostream& operator<<(std::ostream& stream, TokenType const& type)
{
    return stream << tokenTypeRepr[(unsigned char) type];
}


size_t Tokens::typeSize(TokenType type)
{
    switch (type)
    {
    case TokenType::BOOL:
        return 1;
        
    case TokenType::INT:
    case TokenType::FLOAT:
        return 4;
    
    case TokenType::STRING:
    case TokenType::DOUBLE:
    case TokenType::LONG:
    case TokenType::FUNCTION:
        return 8;

    default:
        return 0;
    }
}

