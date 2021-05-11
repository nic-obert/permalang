#include "token.hh"


using namespace Tokens;


std::ostream& operator<<(std::ostream& stream, TokenType const& type)
{
    switch (type)
    {
    case TokenType::NONE:
        return stream << "NONE";
    
    case TokenType::TEXT:
        return stream << "TEXT";

    case TokenType::INT:
        return stream << "INT";

    case TokenType::FLOAT:
        return stream << "FLOAT";
    
    case TokenType::BOOL:
        return stream << "BOOL";
        
    case TokenType::STRING:
        return stream << "STRING";

    case TokenType::KEYWORD:
        return stream << "KEYWORD";
    
    case TokenType::ENDS:
        return stream << "END OF STATEMENT";
    
    default:
        return stream << "UNDEFINED TYPE";
    
    }
}


size_t Tokens::typeSize(TokenType type)
{
    switch (type)
    {
    case TokenType::BOOL:
        return 1;
        
    case TokenType::INT:
        return 4;
    
    case TokenType::STRING:
    case TokenType::FLOAT:
        return 8;

    default:
        return 0;
    }
}

