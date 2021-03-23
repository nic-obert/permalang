
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
    statements = Statements();
    Statement* statement = nullptr;
    Token* token;

    for (Token* tok = tokens->first; tok != nullptr; tok = tok->next)
    {

        // end of statement --> add statement to list of statements
        if (tok->type == ENDS)
        {
            // don't add empty statements
            if (statement->root != nullptr)
            {
                token->next = nullptr;
                statements.add(statement);
                statement = nullptr;
                token = nullptr;
            }
            continue;
        }

        // first token of statement
        if (statement == nullptr)
        {
            tok->prev = nullptr;
            statement = new Statement(tok);
        }

        token = tok;

    }

}


Tokens::Token* SyntaxTree::getHighestPriority(Tokens::Token* root) 
{
    using namespace Tokens;

    // check if first token of statement is null (empty statement)
    if (root == nullptr)
    {
        return nullptr;
    }

    for (Tokens::Token* token = root; token != nullptr; token = token->next)
    {
        if (token->priority > root->priority)
        {
            root = token;
        }
    }

    return root;
}


void SyntaxTree::parse()
{

    // loop through every statement --> for every statement, parse it and build a tree out of it
    for (Statement* statement = statements.start; statement != nullptr; statement = statement->next)
    {
        Tokens::Token* root = statement->root;

        // parse statement
        while (true)
        {   
            
            // return to 
            while (root->prev != nullptr)
            {
                root = root->prev;
            }

            if (root->next == nullptr)
            {
                break;
            }

            root = getHighestPriority(root);

            if (root->priority < 1) // 0 or less
            {
                break;
            }

            root->satisfy();
            
        }

    }

}


std::ostream& operator<<(std::ostream& stream, SyntaxTree const& tree)
{
    for (Statement* statement = tree.statements.start; statement != nullptr; statement = statement->next)
    {
        stream << *statement;
    }

    return stream;
}

