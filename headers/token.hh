#pragma once

#include "pch.hh"

#include "utils.hh"
#include "op_codes.hh"


#define isDigit(x) (47 < x && x < 58)
#define toDigit(x) (x - 48)
#define isText(x) ((64 < x && x < 91) || (x == '_') || (96 < x && x < 123))


namespace Tokens 
{

    // syntactical type of a Token
    // enum values must be constant for lookup tables
    typedef enum class TokenType : unsigned char
    {
        NONE        = 0, // a Token of no type
        TEXT        = 1, // any reference (e.g. variable or function name)
        INT         = 2,
        FLOAT       = 3,
        STRING      = 4,
        KEYWORD     = 5,
        BOOL        = 6,
        SCOPE       = 7, // {}
        PARENTHESIS = 8,
        ENDS        = 9, // end of statement
        DOUBLE      = 10,
        LONG        = 11,

    } TokenType;


    // returns the size of a TokenType
    size_t typeSize(TokenType type);


    // syntactical element of a program
    class Token
    {
    public:

        TokenType type;
        unsigned int priority;
        Value value;

        OpCodes opCode;

        Token* prev = nullptr;
        Token* next = nullptr;

        Token(TokenType type, unsigned int priority, OpCodes opCode, Value value);
        Token(TokenType type, unsigned int priority, OpCodes opCode);

        ~Token();

        // removes the token from where it is located
        // data-structure specific methods are preferred to this
        static void removeToken(Token* token);

    };


    // doubly-linked list of Tokens
    class TokenList
    {
    public:
        Token* first = nullptr;
        Token* last = nullptr;
        

        TokenList(std::string& script);

        // adds the Token to the doubly-linked list 
        void add(Token* token);

        // removes the specified Token from the doubly-linked list
        // does not check if token is actually in list
        void remove(Token* token);

    };

};


std::ostream& operator<<(std::ostream& stream, const Tokens::Token& token);

std::ostream& operator<<(std::ostream& stream, const Tokens::TokenList& tokenList);

std::ostream& operator<<(std::ostream& stream, const Tokens::TokenType& tokenType);

