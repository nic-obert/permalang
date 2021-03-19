
#include "token.hh"
#include "syntax_tree.hh"

#include <iostream>


using namespace syntax_tree;


SyntaxTree::SyntaxTree(Tokens::TokenList* tokens)
{
    using namespace Tokens;

    if (tokens->first == nullptr)
    {
        // TODO implement errors
        return;
    }

    root = new SyntaxNode(tokens->first);
    SyntaxNode* node = root;

    for (Token* tok = tokens->first->next; tok != nullptr; tok = tok->next)
    {
        node->next = new SyntaxNode(tok);
        node = node->next;   
    }

}


SyntaxNode* SyntaxTree::getHighestPriority() 
{
    using namespace Tokens;

    if (root == nullptr)
    {
        return nullptr;
    }

    SyntaxNode* highest = root;
    for (SyntaxNode* node = root; node != nullptr; node = node->next)
    {
        if (node->token->priority > highest->token->priority)
        {
            highest = node;
        }
    }

    return highest;
}


void SyntaxTree::parse()
{

    if (root != nullptr)
    {
        // TODO throw error
        return;
    }

    while (true)
    {   
        
        while (root->prev != nullptr)
        {
            root = root->prev;
        }

        if (root->next == nullptr)
        {
            break;
        }

        root = getHighestPriority();
        root->satisfy();
        
    }
}



void SyntaxTree::print() const
{
    for (SyntaxNode* node = root; node != nullptr; node = node->next)
    {
        node->print();
    }

}

