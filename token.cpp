#pragma once

#include <iostream>

#include "operators/operators.hh"
#include "keywords.cpp"

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
        
        case ARITHMETIC_OP:
            return "ARITHMETIC OPERATOR";
        
        case ASSIGNMENT_OP:
            return "ASSIGNMENT OPERATOR";

        case LOGICAL_OP:
            return "LOGICAL OPERATOR";

        case KEYWORD:
            return "KEYWORD";
        
        default:
            return "UNDEFINED TYPE";
        
        }
    }


    class Token
    {
    public:

        TokenType type;
        int priority;
        ulong value;

        Token* prev = nullptr;
        Token* next = nullptr;

        Token(TokenType type, int priority, ulong value) 
        : type(type), priority(priority), value(value)
        {

        }

        void print() const
        {   
            using namespace operators::arithmetical;
            using namespace operators::logical;
            using namespace operators::assignment;
            using namespace Keywords;


            switch (type)
            {
            case TEXT:
            case STRING:
                std::cout << "<" << tokenTypeName(type) << ": " << *(std::string*)value << " (" << priority << ")>\n";
                break;
            
            case KEYWORD:
                std::cout << "<" << tokenTypeName(type) << ": " << keywordName((Keywords::Keywords) value) << " (" << priority << ")>\n";
                break;
            
            case ARITHMETIC_OP:
                std::cout << "<" << tokenTypeName(type) << ": " << arithmeticalOperatorName((ArithmeticalOperators) value) << " (" << priority << ")>\n";
                break;
            
            case ASSIGNMENT_OP:
                std::cout << "<" << tokenTypeName(type) << ": " << assignmentOperatorName((AssignmentOperators) value) << " (" << priority << ")>\n";
                break;

            case LOGICAL_OP:
                std::cout << "<" << tokenTypeName(type) << ": " << logicalOperatorName((LogicalOperators) value) << " (" << priority << ")>\n";
                break;

            default:
                std::cout << "<" << tokenTypeName(type) << ": " << value << " (" << priority << ")>\n";
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


        Token* getHighestPriority() const 
        {
            if (first == nullptr)
                return nullptr;

            Token* highest = first;
            for (Token* token = first; token != nullptr; token = token->next)
            {
                if (token->priority > highest->priority)
                {
                    highest = token;
                }
            }

            return highest;
        }


        void print() const
        {
            if (first == nullptr)
                std::cout << "Empty token line" << std::endl;

            for (Token* token = first; token != nullptr; token = token->next)
            {
                token->print();
            }
        }

    };

};
