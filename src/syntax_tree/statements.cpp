
#include "syntax_tree.hh"

#include <iostream>


using namespace syntax_tree;


Statement::Statement(Tokens::Token* root)
: root(root)
{

}


Statement::Statement()
{
    
}


void Statement::remove(Tokens::Token* token)
{
    // check first if token is root
    if (token == root)
    {
        root = token->next;
        return;
    }

    if (token->prev != nullptr)
    {
        token->prev->next = token->next;
    }
    if (token->next != nullptr)
    {
        token->next->prev = token->prev;
    }
    
}


std::ostream& operator<<(std::ostream& stream, syntax_tree::Statement const& statement)
{

    for (Tokens::Token* token = statement.root; token != nullptr; token = token->next)
    {
        stream << *token;
    }

    return stream;
}



Statements::Statements(Statement* statement)
{
    start = statement;
    end = start;
}


Statements::Statements()
{
    start = nullptr;
    end = nullptr;
}


void Statements::add(Statement* statement)
{

    if (end == nullptr)
    {
        start = statement;
        end = start;
        return;
    }

    end->next = statement;
    end = end->next;
}


