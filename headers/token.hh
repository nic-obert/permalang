#pragma once

#include <string>


#define isDigit(x) (47 < x && x < 58)
#define toDigit(x) (x - 48)
#define isText(x) ((64 < x && x < 91) || (x == '_') || (96 < x && x < 123))


namespace Tokens 
{

    typedef enum TokenType
    {
        NONE,
        TEXT,
        NUMBER,
        STRING,
        ARITHMETIC_OP,
        ASSIGNMENT_OP,
        LOGICAL_OP,
        KEYWORD

    } TokenType;


    class Token
    {
    public:

        TokenType type;
        int priority;
        unsigned long value;

        Token* prev = nullptr;
        Token* next = nullptr;

        Token(TokenType type, int priority, unsigned long value);

        void print() const;


        void satisfy();

    };


    const char* tokenTypeName(TokenType type);


    class TokenList
    {
    public:
        Token* first = nullptr;
        Token* last = nullptr;
        

        TokenList();


        void add(Token* token);


        void remove(Token* token);


        Token* getHighestPriority() const;


        void print() const;

    };


    TokenList* tokenize(std::string script);
    

};
