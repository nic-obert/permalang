
#include <iostream>

#include "token.hh"
#include "operators.hh"
#include "keywords.hh"
#include "utils.hh"
#include "op_codes.hh"


using namespace Tokens;





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


Token::Token(TokenType type, int priority, OpCodes opCode, Value value) 
: type(type), priority(priority), value(value), opCode(opCode)
{
    
}


Token::Token(TokenType type, int priority, OpCodes opCode) 
: type(type), priority(priority), value(0), opCode(opCode)
{
    
}


std::ostream& operator<<(std::ostream& stream, Token const& token)
{   

    switch (token.opCode)
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
        
    default:
    {
        
        if (token.type == KEYWORD) 
        {
            stream << "<" << token.type << ": " << Keywords::keywordName(token.opCode) << " (" << token.priority << ")>";
            return stream;
        }

        if (isArithmeticalOp(token.type))
        {
            using namespace operators::arithmetical;
            stream << "<" << token.type << ": " << arithmeticalOperatorName((operators::arithmetical::ArithmeticalOperators) token.value) << " (" << token.priority << ")>";
            return stream;
        }

        if (isAssignmentOp(token.type))
        {
            using namespace operators::assignment;
            stream << "<" << token.type << ": " << assignmentOperatorName((AssignmentOperators) token.value) << " (" << token.priority << ")>";
            return stream;
        }

        if (isLogicalOp(token.type))
        {
            using namespace operators::logical;
            stream << "<" << token.type << ": " << logicalOperatorName((LogicalOperators) token.value) << " (" << token.priority << ")>";
            return stream;
        }
        
        break;
    } // default

    } // switch (token.opCode)


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

