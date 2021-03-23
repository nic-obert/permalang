
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


