
#include <iostream>

#include "token.hh"
#include "operators.hh"
#include "keywords.hh"


std::ostream& operator<<(std::ostream& stream, Tokens::TokenType const& type)
{

    using namespace Tokens;

    switch (type)
    {
    case NONE:
        return stream << "NONE";
    
    case TEXT:
        return stream << "TEXT";

    case NUMBER:
        return stream << "NUMBER";
        
    case STRING:
        return stream << "STRING";
    
    case ARITHMETIC_OP:
        return stream << "ARITHMETIC OPERATOR";
    
    case ASSIGNMENT_OP:
        return stream << "ASSIGNMENT OPERATOR";

    case LOGICAL_OP:
        return stream << "LOGICAL OPERATOR";

    case KEYWORD:
        return stream << "KEYWORD";
    
    case ENDS:
        return stream << "END OF STATEMENT";
    
    default:
        return stream << "UNDEFINED TYPE";
    
    }
}


Tokens::Token::Token(TokenType type, int priority, unsigned long value) 
: type(type), priority(priority), value(value)
{

}

std::ostream& operator<<(std::ostream& stream, Tokens::Token const& token)
{   

    using namespace Tokens;

    switch (token.type)
    {
    case TEXT:
    case STRING: {
        stream << "<" << token.type << ": " << *(std::string*)token.value << " (" << token.priority << ")>";
        break;
    }
    case KEYWORD: {
        stream << "<" << token.type << ": " << keywordName((Keywords::Keywords) token.value) << " (" << token.priority << ")>";
        break;
    }
    case ARITHMETIC_OP: {
        using namespace operators::arithmetical;
        stream << "<" << token.type << ": " << arithmeticalOperatorName((operators::arithmetical::ArithmeticalOperators) token.value) << " (" << token.priority << ")>";
        break;
    }
    case ASSIGNMENT_OP: {
        using namespace operators::assignment;
        stream << "<" << token.type << ": " << assignmentOperatorName((AssignmentOperators) token.value) << " (" << token.priority << ")>";
        break;
    }
    case LOGICAL_OP: {
        using namespace operators::logical;
        stream << "<" << token.type << ": " << logicalOperatorName((LogicalOperators) token.value) << " (" << token.priority << ")>";
        break;
    }
    default:
        stream << "<" << token.type << ": " << token.value << " (" << token.priority << ")>";
        break;
    }

    return stream;
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


std::ostream& operator<<(std::ostream& stream, Tokens::TokenList const& list)
{
    stream << "{\n";
    if (list.first == nullptr)
        stream << "\tEmpty token line\n";

    for (Tokens::Token* token = list.first; token != nullptr; token = token->next)
    {
        stream << "\t" << *token << "\n";
    }

    stream << "}\n";
}


std::ostream& operator<<(std::ostream& stream, Tokens::OperatorType const& type)
{

    using namespace Tokens;

    switch (type)
    {
    case REFERENCE:
        return stream << "REFERENCE";
    
    case LITERAL:
        return stream << "LITERAL";

    case BINARY:
        return stream << "OPERATOR_BINARY";
    
    case UNARY:
        return stream << "OPERATOR_UNARY";
    }

}


