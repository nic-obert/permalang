
#include <iostream>

#include "token.hh"
#include "operators.hh"
#include "keywords.hh"


using namespace Tokens;


OperatorType operatorTypeOfToken(Token* token)
{
    switch (token->type)
    {
    // operators
    case Tokens::KEYWORD: 
    {
        switch ((Keywords::Keywords) token->value)
        {
        case Keywords::Keywords::IF:
            return OperatorType::BINARY;

        case Keywords::Keywords::ELSE:
        case Keywords::Keywords::BOOL:
        case Keywords::Keywords::FLOAT:
        case Keywords::Keywords::INT:
        case Keywords::Keywords::STRING:
            return OperatorType::UNARY;
        }
    }

    case Tokens::ARITHMETIC_OP: 
    {
        using namespace operators::arithmetical;

        switch ((ArithmeticalOperators) token->value)
        {
        case ArithmeticalOperators::SUM:
        case ArithmeticalOperators::DIVISION:
        case ArithmeticalOperators::MULTIPLICATION:
        case ArithmeticalOperators::SUBTRACTION:
        case ArithmeticalOperators::POWER:
            return OperatorType::UNARY;
        
        case ArithmeticalOperators::INCREMENT:
        case ArithmeticalOperators::DECREMENT:
            return OperatorType::UNARY;
        }
    }

    case Tokens::LOGICAL_OP: 
    {
        using namespace operators::logical;

        switch ((LogicalOperators) token->value)
        {
        case LogicalOperators::AND:
        case LogicalOperators::OR:
        case LogicalOperators::EQUALITY:
        case LogicalOperators::GREATER_EQUAL:
        case LogicalOperators::INEQUALITY:
        case LogicalOperators::LESS_EQUAL:
        case LogicalOperators::GREATER_THAN:
        case LogicalOperators::LESS_THAN:
            return OperatorType::BINARY;
        
        case LogicalOperators::NOT:
            return OperatorType::UNARY;
        }
    }

    case Tokens::ASSIGNMENT_OP:
    {
        return OperatorType::BINARY;
    }
    
    // literals
    case Tokens::BOOL:
    case Tokens::INT:
    case Tokens::FLOAT:
    case Tokens::STRING:
        return OperatorType::LITERAL;
    
    // references
    case Tokens::TEXT:
        return OperatorType::REFERENCE;
        
    }
}


std::ostream& operator<<(std::ostream& stream, TokenType const& type)
{

    switch (type)
    {
    case NONE:
        return stream << "NONE";
    
    case TEXT:
        return stream << "TEXT";

    case INT:
        return stream << "INT";

    case FLOAT:
        return stream << "FLOAT";
        
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


Token::Token(TokenType type, int priority, unsigned long value) 
: type(type), priority(priority), value(value)
{
    operatorType = operatorTypeOfToken(this);
}


std::ostream& operator<<(std::ostream& stream, Token const& token)
{   

    switch (token.operatorType)
    {
    case LITERAL:
    {
        switch (token.type)
        {
        case TEXT:
        case STRING: 
        {
            stream << "<" << token.type << ": " << *((std::string*) token.value) << " (" << token.priority << ")>";
            return stream;
        }
        }

        break;
    }
    
    case REFERENCE:
    {

        switch (token.type)
        {
        case INT:
        case STRING:
        case BOOL:
        case FLOAT:
            stream << "<" << token.type << "*: " << *(std::string*) token.value << " (" << token.priority << ")>";
            return stream;
        
        case TEXT:
            stream << "<" << NONE << "*: " << *(std::string*) token.value << " (" << token.priority << ")>";
            return stream;
        }

        break;
    }
        
    case UNARY:
    case BINARY:
    {
        
        switch (token.type)
        {
        case KEYWORD: 
        {
            stream << "<" << token.type << ": " << keywordName((Keywords::Keywords) token.value) << " (" << token.priority << ")>";
            return stream;
        }

        case ARITHMETIC_OP:
        {
            using namespace operators::arithmetical;
            stream << "<" << token.type << ": " << arithmeticalOperatorName((operators::arithmetical::ArithmeticalOperators) token.value) << " (" << token.priority << ")>";
            return stream;
        }

        case ASSIGNMENT_OP: 
        {
            using namespace operators::assignment;
            stream << "<" << token.type << ": " << assignmentOperatorName((AssignmentOperators) token.value) << " (" << token.priority << ")>";
            return stream;
        }

        case LOGICAL_OP: 
        {
            using namespace operators::logical;
            stream << "<" << token.type << ": " << logicalOperatorName((LogicalOperators) token.value) << " (" << token.priority << ")>";
            return stream;
        }

        }
        
        break;
    }

    }


    // if not returned yet
    stream << "<" << token.type << ": " << token.value << " (" << token.priority << ")>";

    return stream;
}


TokenList::TokenList() {};


void TokenList::add(Token* token) 
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


void TokenList::remove(Token* token)
{
    token->prev->next = token->next;
    token->next->prev = token->prev;

    delete token;
}


std::ostream& operator<<(std::ostream& stream, TokenList const& list)
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


std::ostream& operator<<(std::ostream& stream, OperatorType const& type)
{

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


