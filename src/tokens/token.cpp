
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
    case TokenType::NONE:
        return stream << "NONE";
    
    case TokenType::TEXT:
        return stream << "TEXT";

    case TokenType::INT:
        return stream << "INT";

    case TokenType::FLOAT:
        return stream << "FLOAT";
    
    case TokenType::BOOL:
        return stream << "BOOL";
        
    case TokenType::STRING:
        return stream << "STRING";

    case TokenType::KEYWORD:
        return stream << "KEYWORD";
    
    case TokenType::ENDS:
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
    case OpCodes::LITERAL:
    {
        switch (token.type)
        {
        case TokenType::TEXT:
        case TokenType::STRING: 
        {
            stream << "<" << token.type << ": " << *((std::string*) token.value) << " (" << token.priority << ")>";
            return stream;
        }

        case TokenType::BOOL:
        {
            stream << "<" << token.type << ": " << ((token.value == 0) ? "false" : "true") << " (" << token.priority << ")>";
            return stream;
        }

        } // switch (token.type)

        break;
    } // case LITERAL

    case OpCodes::CALL:
    {
        return stream << "<CALL: (" << token.priority << ")>";
    } // case CALL
    
    case OpCodes::REFERENCE:
    {

        switch (token.type)
        {
        case TokenType::INT:
        case TokenType::STRING:
        case TokenType::BOOL:
        case TokenType::FLOAT:
            stream << "<" << token.type << "*: " << *(std::string*) token.value << " (" << token.priority << ")>";
            return stream;
        
        case TokenType::TEXT:
            stream << "<" << TokenType::NONE << "*: " << *(std::string*) token.value << " (" << token.priority << ")>";
            return stream;
        }

        break;
    } // case REFERENCE
        
    default:
    {
        
        if (token.type == TokenType::KEYWORD) 
        {
            stream << "<" << TokenType::KEYWORD << ": " << Keywords::keywordName(token.opCode) << " (" << token.priority << ")>";
            return stream;
        }

        if (isArithmeticalOp(token.opCode))
        {
            using namespace operators::arithmetical;
            stream << "<ARITHMETICAL OPERATOR: " << arithmeticalOperatorName(token.opCode) << " (" << token.priority << ")>";
            return stream;
        }

        if (isAssignmentOp(token.opCode))
        {
            using namespace operators::assignment;
            stream << "<ASSIGNMENT OPERATOR: " << assignmentOperatorName(token.opCode) << " (" << token.priority << ")>";
            return stream;
        }

        if (isLogicalOp(token.opCode))
        {
            using namespace operators::logical;
            stream << "<LOGICAL OPERATOR: " << logicalOperatorName(token.opCode) << " (" << token.priority << ")>";
            return stream;
        }

        if (token.opCode == OpCodes::ADDRESS_OF)
        {
            stream << "<ADDRESS OF: & (" << token.priority << ")>";
            return stream;
        }

        if (token.type == TokenType::PARENTHESIS)
        {
            return stream << "<PARENTHESIS: " << (char) token.value << " (" << token.priority << ")>";
        }

        if (token.type == TokenType::ENDS)
        {
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
    stream << "Token List: {\n";
    if (list.first == nullptr)
        stream << "\tEmpty token line\n";

    for (Tokens::Token* token = list.first; token != nullptr; token = token->next)
    {
        stream << "\t" << *token << "\n";
    }

    stream << "}";
}

