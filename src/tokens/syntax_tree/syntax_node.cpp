
#include <iostream>

#include "syntax_tree.hh"
#include "token.hh"
#include "operators.hh"
#include "utils.hh"
#include "keywords.hh"


#define RIGHT 0
#define LEFT 1

using namespace syntax_tree;


std::ostream& operator<<(std::ostream& stream, syntax_tree::SyntaxType const& type)
{

    switch (type)
    {
    case REFERENCE:
        return stream << "REFERENCE";
    
    case LITERAL:
        return stream << "LITERAL";

    case OPERATOR_BINARY:
        return stream << "OPERATOR_BINARY";
    
    case OPERATOR_UNARY:
        return stream << "OPERATOR_UNARY";
    }

}



SyntaxType syntaxTypeOfToken(Tokens::Token* token)
{
    switch (token->type)
    {
    // operators
    case Tokens::KEYWORD: 
    {
        switch ((Keywords::Keywords) token->value)
        {
        case Keywords::Keywords::BOOL:
        case Keywords::Keywords::FLOAT:
        case Keywords::Keywords::INT:
        case Keywords::Keywords::STRING:
        case Keywords::Keywords::IF:
            return OPERATOR_BINARY;

        case Keywords::Keywords::ELSE:
            return OPERATOR_UNARY;
        }
        break;
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
            return OPERATOR_UNARY;
        
        case ArithmeticalOperators::INCREMENT:
        case ArithmeticalOperators::DECREMENT:
            return OPERATOR_UNARY;
        }
        break;
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
            return OPERATOR_BINARY;
        
        case LogicalOperators::NOT:
            return OPERATOR_UNARY;
        }
        break;
    }

    case Tokens::ASSIGNMENT_OP:
    {
        return OPERATOR_BINARY;
    }
    
    // literals
    case Tokens::BOOL:
    case Tokens::NUMBER:
    case Tokens::STRING:
        return LITERAL;
    
    // references
    case Tokens::TEXT:
        return REFERENCE;
        
    }
}


SyntaxNode::SyntaxNode(Tokens::Token* token)
: token(token)
{
    value = token->value;
    type = syntaxTypeOfToken(token);
}


SyntaxNode::SyntaxNode(Tokens::Token* token, SyntaxNode* prev)
: token(token), prev(prev)
{
    value = token->value;
    type = syntaxTypeOfToken(token);
}


void binarySatisfy(SyntaxNode* node, Tokens::TokenType leftType, Tokens::TokenType rightType)
{
    using namespace Tokens;

    if (node->prev == nullptr)
    {
        std::cerr << "Missing token of type " << leftType << " to the left of " << *node << std::endl;
        exit(1);
    }    
    else if (node->next == nullptr)
    {
        std::cerr << "Missing token of type " << rightType << " to the right of " << *node << std::endl;
        exit(1);
    }

    if (node->prev->token->type != leftType)
    {
        std::cerr << "Token " << *node << " requires token of type " << leftType << " to the left, but " << node->prev->token->type << " was provided" << std::endl;
            exit(1);
    }
    else if (node->next->token->type != rightType)
    {
        std::cerr << "Token " << *node << " requires token of type " << rightType << " to the right, but " << node->prev->token->type << " was provided" << std::endl;
        exit(1);
    }


    node->prev->parent = node;
    node->next->parent = node;

    node->prev->next = nullptr;
    node->prev->prev = nullptr;
    node->next->next = nullptr;
    node->next->prev = nullptr;

    node->value = (Value) new SyntaxNode*[2] {node->prev, node->next};

}


void unarySatisfy(SyntaxNode* node, Tokens::TokenType type, char side)
{

    using namespace Tokens;

    if (side == LEFT)
    {
        if (node->prev == nullptr)
        {
            std::cerr << "Missing token of type " << type << " to the left of " << *node << std::endl;
            exit(1);
        }
        if (node->prev->token->type != type)
        {
            std::cerr << "Token " << *node << " requires token of type " << type << " to the left, but " << node->prev->token->type << " was provided" << std::endl;
            exit(1);
        }

        node->prev->parent = node;
        node->prev->next = nullptr;
        node->prev->prev = nullptr;

        node->value = (Value) node->prev;

    } 
    else 
    {
        if (node->next == nullptr)
        {
            std::cerr << "Missing token of type " << type << " to the right of " << *node << std::endl;
            exit(1);
        }
        if (node->next->token->type != type)
        {
            std::cerr << "Token " << *node << " requires token of type " << type << " to the left, but " << node->prev->token->type << " was provided" << std::endl;
            exit(1);
        }

        node->next->parent = node;
        node->next->next = nullptr;
        node->next->prev = nullptr;

        node->value = (Value) node->next;

    }

}


void SyntaxNode::satisfy()
{
    using namespace Tokens;

    // set SyntaxNode's priority to 0 right away so that it doesn't get evaluated multiple times
    token->priority = 0;

    switch (token->type)
    {
    case ARITHMETIC_OP: {

        using namespace operators::arithmetical;

        switch (token->value)
        {
        case POWER:
        case DIVISION:
        case MULTIPLICATION:
        case SUBTRACTION:
        case SUM:
            binarySatisfy(this, NUMBER, NUMBER);
            break;
        
        case INCREMENT:
        case DECREMENT:
            unarySatisfy(this, NUMBER, LEFT);
            break;
        
        }
        break;
    }
    
    case LOGICAL_OP: {

        using namespace operators::logical;

        switch (token->value)
        {
        case EQUALITY:
        case INEQUALITY:
        case AND:
        case OR:
        case GREATER_THAN:
        case GREATER_EQUAL:
        case LESS_THAN:
        case LESS_EQUAL:
            binarySatisfy(this, BOOL, BOOL);
            break;

        case NOT:
            unarySatisfy(this, BOOL, RIGHT);
            break;
        }

        break;
    }

    case ASSIGNMENT_OP: {

        using namespace operators::assignment;

        switch (token->value)
        {
        case ASSIGNMENT:
        case ADD:
        case SUBTRACT:
        case DIVIDE:
        case ELEVATE:
        case MULTIPLY:
            binarySatisfy(this, TEXT, NUMBER);
            break;
        
        default:
            break;
        }

        break;
    }

    }

}


std::ostream& operator<<(std::ostream& stream, SyntaxNode const& node)
{
    return stream << node;
}

