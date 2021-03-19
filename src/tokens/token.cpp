
#include <iostream>

#include "token.hh"
#include "operators.hh"
#include "keywords.hh"


const char* Tokens::tokenTypeName(TokenType type)
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


Tokens::Token::Token(TokenType type, int priority, unsigned long value) 
: type(type), priority(priority), value(value)
{

}

void Tokens::Token::print() const
{   

    switch (type)
    {
    case TEXT:
    case STRING: {
        std::cout << "<" << tokenTypeName(type) << ": " << *(std::string*)value << " (" << priority << ")>\n";
        break;
    }
    case KEYWORD: {
        std::cout << "<" << tokenTypeName(type) << ": " << keywordName((Keywords::Keywords) value) << " (" << priority << ")>\n";
        break;
    }
    case ARITHMETIC_OP: {
        using namespace operators::arithmetical;
        std::cout << "<" << tokenTypeName(type) << ": " << arithmeticalOperatorName((operators::arithmetical::ArithmeticalOperators) value) << " (" << priority << ")>\n";
        break;
    }
    case ASSIGNMENT_OP: {
        using namespace operators::assignment;
        std::cout << "<" << tokenTypeName(type) << ": " << assignmentOperatorName((AssignmentOperators) value) << " (" << priority << ")>\n";
        break;
    }
    case LOGICAL_OP: {
        using namespace operators::logical;
        std::cout << "<" << tokenTypeName(type) << ": " << logicalOperatorName((LogicalOperators) value) << " (" << priority << ")>\n";
        break;
    }
    default:
        std::cout << "<" << tokenTypeName(type) << ": " << value << " (" << priority << ")>\n";
        break;
    }
}


void Tokens::Token::satisfy()
{
    using namespace operators;

    switch (type)
    {
        case ARITHMETIC_OP:
            arithmetical::satisfy(this);
        
        
    }
}





Tokens::TokenList::TokenList() {};


void Tokens::TokenList::add(Token* token) 
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


void Tokens::TokenList::remove(Token* token)
{
    token->prev->next = token->next;
    token->next->prev = token->prev;

    delete token;
}


Tokens::Token* Tokens::TokenList::getHighestPriority() const 
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


void Tokens::TokenList::print() const
{
    if (first == nullptr)
        std::cout << "Empty token line" << std::endl;

    for (Token* token = first; token != nullptr; token = token->next)
    {
        token->print();
    }
}


