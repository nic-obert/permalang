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
        NONE, // a Token of no type
        TEXT, // any reference (e.g. variable or function name)
        INT,
        FLOAT,
        STRING,
        KEYWORD,
        BOOL,
        SCOPE, // {}
        PARENTHESIS,
        ENDS, // end of statement
        DOUBLE,
        LONG,
        FUNCTION,
        COMMA,
        NUMERIC,
        NO_TOK,
        BYTE,

    } TokenType;


    // whether the first TokenType is compatible with the second
    bool isCompatible(TokenType got, TokenType required);


    // returns the size of a TokenType
    unsigned char typeSize(TokenType type);


    // syntactical element of a program
    class Token
    {
    public:

        TokenType type;
        size_t priority;
        Value value;

        OpCodes opCode;

        Token* prev = nullptr;
        Token* next = nullptr;

        Token(TokenType type, size_t priority, OpCodes opCode, Value value);
        Token(TokenType type, size_t priority, OpCodes opCode);

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


    /*
    - returns the TokenType of a given token
    - returns the TokenType of it's value if token is a variable
    - throws exception if token is not declared
    */
    TokenType tokenTypeOf(const Token* token);


    // copies all releveant data during parsing from a token to another
    void copyRelevantData(Token* dest, const Token* src);

};


std::ostream& operator<<(std::ostream& stream, const Tokens::Token& token);

std::ostream& operator<<(std::ostream& stream, const Tokens::TokenList& tokenList);

std::ostream& operator<<(std::ostream& stream, const Tokens::TokenType& tokenType);

