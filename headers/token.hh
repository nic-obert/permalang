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
    typedef enum class TokenType
    {
        NONE,           // a Token of no type
        TEXT,           // any reference (e.g. variable or function name)
        INT,
        FLOAT,
        STRING,
        KEYWORD,
        BOOL,
        SCOPE,          // {}
        PARENTHESIS,
        ENDS,           // end of statement
    } TokenType;


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

        // token destructor
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


std::ostream& operator<<(std::ostream& stream, Tokens::Token const& token);

std::ostream& operator<<(std::ostream& stream, Tokens::TokenList const& token);

std::ostream& operator<<(std::ostream& stream, Tokens::TokenType const& token);

