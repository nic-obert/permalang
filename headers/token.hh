#pragma once

#include <string>
#include <iostream>

#include "utils.hh"
#include "op_codes.hh"


#define isDigit(x) (47 < x && x < 58)
#define toDigit(x) (x - 48)
#define isText(x) ((64 < x && x < 91) || (x == '_') || (96 < x && x < 123))


namespace Tokens 
{

    typedef enum TokenType
    {
        NONE,
        TEXT,
        INT,
        FLOAT,
        STRING,
        KEYWORD,
        BOOL,
        ENDS // end of statement
    } TokenType;


    class Token
    {
    public:

        TokenType type;
        int priority;
        Value value;

        OpCodes opCode;

        Token* parent = nullptr;

        Token* prev = nullptr;
        Token* next = nullptr;

        Token(TokenType type, int priority, OpCodes opCode, Value value);
        Token(TokenType type, int priority, OpCodes opCode);

    };


    class TokenList
    {
    public:
        Token* first = nullptr;
        Token* last = nullptr;
        

        TokenList();


        void add(Token* token);


        void remove(Token* token);

    };


    TokenList* tokenize(std::string& script);
    

};


std::ostream& operator<<(std::ostream& stream, Tokens::Token const& token);

std::ostream& operator<<(std::ostream& stream, Tokens::TokenList const& token);

std::ostream& operator<<(std::ostream& stream, Tokens::TokenType const& token);

