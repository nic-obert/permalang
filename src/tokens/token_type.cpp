#include "token.hh"
#include "symbol_table.hh"


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
    "FUNCTION",
    "COMMA",
    "NUMERIC"
};


std::ostream& operator<<(std::ostream& stream, TokenType const& type)
{
    return stream << tokenTypeRepr[(unsigned char) type];
}


unsigned char Tokens::typeSize(TokenType type)
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


TokenType Tokens::tokenTypeOf(const Token* token)
{
    if (token->opCode == OpCodes::REFERENCE)
    {
        return symbol_table::SymbolTable::get((std::string*) token->value)->type;
    }
    
    return token->type;
}


// arrays of compatible types

static const TokenType noneCompatible[] = {TokenType::NONE, TokenType::NO_TOK};
static const TokenType textCompatible[] = {TokenType::TEXT, TokenType::NO_TOK};
static const TokenType intCompatible[] = {TokenType::INT, TokenType::BOOL, TokenType::NO_TOK};
static const TokenType floatCompatible[] = {TokenType::INT, TokenType::FLOAT, TokenType::BOOL, TokenType::NO_TOK};
static const TokenType stringCompatible[] = {TokenType::STRING, TokenType::NO_TOK};
static const TokenType keywordCompatible[] = {TokenType::KEYWORD, TokenType::NO_TOK};
static const TokenType boolCompatible[] = {TokenType::INT, TokenType::BOOL, TokenType::FLOAT, TokenType::DOUBLE, TokenType::LONG, TokenType::NO_TOK};
static const TokenType scopeCompatible[] = {TokenType::SCOPE, TokenType::NO_TOK};
static const TokenType parenthesisCompatible[] = {TokenType::PARENTHESIS, TokenType::NO_TOK};
static const TokenType endsCompatible[] = {TokenType::ENDS, TokenType::NO_TOK};
static const TokenType doubleCompatible[] = {TokenType::DOUBLE, TokenType::INT, TokenType::BOOL, TokenType::LONG, TokenType::FLOAT, TokenType::NO_TOK};
static const TokenType longCompatible[] = {TokenType::LONG, TokenType::INT, TokenType::BOOL, TokenType::NO_TOK};
static const TokenType functionCompatible[] = {TokenType::FUNCTION, TokenType::NO_TOK};
static const TokenType commaCompatible[] = {TokenType::COMMA, TokenType::NO_TOK};
static const TokenType numericCompatible[] = {TokenType::NUMERIC, TokenType::BOOL, TokenType::INT, TokenType::FLOAT, TokenType::DOUBLE, TokenType::LONG, TokenType::NO_TOK};


static const TokenType* const compatibles[] =
{

    noneCompatible,
    textCompatible,
    intCompatible,
    floatCompatible,
    stringCompatible,
    keywordCompatible,
    boolCompatible,
    scopeCompatible,
    parenthesisCompatible,
    endsCompatible,
    doubleCompatible,
    longCompatible,
    functionCompatible,
    commaCompatible,
    numericCompatible

};


bool Tokens::isCompatible(TokenType got, TokenType required)
{
    const TokenType* const array = compatibles[(unsigned char) required];

    for (unsigned char i = 0; array[i] != TokenType::NO_TOK; i++)
    {
        if (got == array[i])
        {
            return true;
        }
    }

    return false;
}

