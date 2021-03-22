
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

    // start of script

    SyntaxNode* statement = new SyntaxNode(tokens->first); // first node in the statement (linked list)
    SyntaxNode* node = statement; // current (last) node in statement linked list

    statements = Statements();

    for (Token* tok = tokens->first->next; tok != nullptr; tok = tok->next)
    {

        // end of statement --> add statement to list of statements
        if (tok->type == ENDS)
        {
            // don't add empty statements
            if (statement != nullptr)
            {
                statements.add(statement);
                statement = nullptr;
                node = nullptr;
            }
            continue;
        }

        // first token of statement
        if (statement == nullptr)
        {
            statement = new SyntaxNode(tok);
            node = statement;
            continue;
        }

        // if token is not the first of statement
        node->next = new SyntaxNode(tok, node);
        node = node->next;   
    }

}


SyntaxNode* SyntaxTree::getHighestPriority(SyntaxNode* root) 
{
    using namespace Tokens;

    // check if first token of statement is null (empty statement)
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

    // loop through every statement --> for every statement, parse it and build a tree out of it
    for (Statement* statement = statements.start; statement != nullptr; statement = statement->next)
    {
        SyntaxNode* root = statement->root;

        if (root == nullptr)
        {
            // TODO throw error
            return;
        }

        // parse statement
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

            root = getHighestPriority(root);

            if (root->token->priority < 1) // 0 or less
            {
                break;
            }

            root->satisfy();
            
        }

    }

}

