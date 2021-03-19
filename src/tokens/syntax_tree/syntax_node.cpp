
#include <iostream>

#include "syntax_tree.hh"
#include "token.hh"
#include "operators.hh"
#include "utils.hh"


#define RIGHT 0
#define LEFT 1

using namespace syntax_tree;


SyntaxNode::SyntaxNode(Tokens::Token* token)
: token(token)
{
    value = token->value;
}


void SyntaxNode::satisfy()
{
    using namespace Tokens;

    switch (token->type)
    {
    case ARITHMETIC_OP: {

        using namespace operators::arithmetical;

        switch (token->value)
        {
        case SUM:
            
            

            break;
        
        default:
            break;
        }
        break;
    }
        

    }

}


void binarySatisfy(SyntaxNode* node, Tokens::TokenType leftType, Tokens::TokenType rightType)
{
    using namespace Tokens;

    if (node->prev == nullptr)
    {
        std::cerr << "Missing token of type " << tokenTypeName(leftType) << " to the left of " << node << std::endl;
        exit(1);
    }    
    else if (node->next == nullptr)
    {
        std::cerr << "Missing token of type " << tokenTypeName(rightType) << " to the right of " << node << std::endl;
        exit(1);
    }


    node->prev->parent = node;
    node->next->parent = node;

    node->value = (Value) new SyntaxNode*[2] {node->prev, node->next};

}


void unarySatisfy(SyntaxNode* node, Tokens::TokenType type, char side)
{

    using namespace Tokens;

    if (side == LEFT)
    {
        if (node->prev == nullptr)
        {
            std::cerr << "Missing token of type " << tokenTypeName(type) << " to the left of " << node << std::endl;
            exit(1);
        }
        if (node->prev->token->type != type)
        {
            std::cerr << "Token " << node << " requires token of type " << tokenTypeName(type) << " to the left, but " << tokenTypeName(node->prev->token->type) << " was provided" << std::endl;
            exit(1);
        }
    } 
    else 
    {
        if (node->next == nullptr)
        {
            std::cerr << "Missing token of type " << tokenTypeName(type) << " to the right of " << node << std::endl;
            exit(1);
        }
        if (node->next->token->type != type)
        {
            std::cerr << "Token " << node << " requires token of type " << tokenTypeName(type) << " to the left, but " << tokenTypeName(node->prev->token->type) << " was provided" << std::endl;
            exit(1);
        }
    }

}


void SyntaxNode::print() const
{

}

