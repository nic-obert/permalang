
#include "token.hh"
#include "syntax_tree.hh"

#include <iostream>


using namespace syntax_tree;


SyntaxTree::SyntaxTree(Statements& statements)
: statements(statements)
{

}



SyntaxTree::SyntaxTree(Tokens::TokenList* tokens)
{
    using namespace Tokens;

    if (tokens->first == nullptr)
    {
        // TODO implement errors

        return;
    }

    // start of script
    statements = Statements(); // linked list of Statement
    Statement* statement = nullptr;
    Token* token;

    // transform the TokenList to a list of Statement
    for (Token* tok = tokens->first; tok != nullptr; tok = tok->next)
    {

        // end of statement --> add statement to list of statements
        if (tok->type == TokenType::ENDS)
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


// returns the token with the highest priority in the statement
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
        // do not go past the current scope
        if (isScope(token->opCode))
        {
            break;
        }

        if (token->priority > root->priority)
        {
            root = token;
        }
    }

    // return token with highest priority
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
            
            // return to the beginning of the statement
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

            // satisfy token's requirements
            statement->satisfy(root);
            
        }

    }

}


std::ostream& operator<<(std::ostream& stream, SyntaxTree const& tree)
{   
    stream << "Syntax Tree: {\n";

    for (Statement* statement = tree.statements.start; statement != nullptr; statement = statement->next)
    {
        stream << "\t" << *statement << "\n";
    }

    return stream << "}";
}

