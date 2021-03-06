#pragma once

#include <iostream>


typedef enum TokenType {
    NONE,
    TEXT,
    NUMBER,
    STRING,
    ARITHMETIC_OPERATOR
} TokenType;


const char* tokenTypeName(TokenType type)
{
    switch (type)
    {
    case NONE:
        return "NONE";
    
    case TEXT:
        return "TEXT";

    case NUMBER:
        return "NUMBER";
        
    case STRING:
        return "STRING";
    
    case ARITHMETIC_OPERATOR:
        return "ARITHMETIC OPERATOR";
    
    default:
        return "UNDEFINED TYPE";
    }
}


class Token
{
public:

    TokenType type;
    int priority;
    unsigned long value;

    Token* prev = nullptr;
    Token* next = nullptr;

    Token(TokenType type, int priority, unsigned long value) 
    : type(type), priority(priority), value(value)
    {

    }

    void print() const
    {
        switch (type)
        {
        case STRING:
            std::cout << "<" << tokenTypeName(type) << ": " << *(std::string*)value << "(" << priority << ")\n";
            break;
        
        case ARITHMETIC_OPERATOR:
            std::cout << "<" << tokenTypeName(type) << ": " << (char)value << "(" << priority << ")\n";
            break;

        default:
            std::cout << "<" << tokenTypeName(type) << ": " << value << "(" << priority << ")\n";
            break;
        }
    }

};


class TokenList
{
public:
    Token* first = nullptr;
    Token* last = nullptr;
    

    TokenList() {

    };


    void add(Token* token) 
    {   
        if (first == nullptr)
        {
            first = token;
            last = first;
            return;
        }
        last->next = token;
        token->prev = last;
        last = token;
    }


    void remove(Token* token)
    {
        token->prev->next = token->next;
        token->next->prev = token->prev;

        delete token;
    }


    void print() const
    {
        if (first == nullptr)
            std::cout << "Empty token line";

        for (Token* token = first; token != nullptr; token = token->next)
        {
            token->print();
        }
    }

};
