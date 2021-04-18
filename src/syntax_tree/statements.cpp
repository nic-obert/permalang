
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


Tokens::Token* Statement::getLast() const
{
    using namespace Tokens;
    
    // loop until last token of the statement
    Token* token;
    for (token = root; token->next != nullptr; token = token->next);

    return token;
}


void Statement::remove(Tokens::Token* token, bool del)
{
    // check first if token is root
    if (token == root)
    {
        if (del)
        {
            delete root;
        }
        
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
    
    if (del)
    {
        delete token;
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


void Statements::removeLast()
{   
    // if there is only 1 statement remove it right away
    if (start == end)
    {
        start = nullptr;
        end = nullptr;
        return;
    }

    Statement* statement;
    // iterate until the second last statement
    for (statement = start; statement->next != end; statement = statement->next);

    // break the linked list
    statement->next = nullptr;
    end = statement;
}


