
#include "syntax_tree.hh"


using namespace syntax_tree;


Statement::Statement(SyntaxNode* root)
: root(root)
{

}


Statement::Statement(SyntaxNode* root, Statement* next)
: root(root), next(next)
{
    
}


Statements::Statements(SyntaxNode* root)
{
    start = new Statement(root);
    end = start;
}


Statements::Statements()
{
    start = nullptr;
    end = nullptr;
}


void Statements::add(SyntaxNode* root)
{

    if (end == nullptr)
    {
        start = new Statement(root);
        end = start;
        return;
    }

    end->next = new Statement(root);
    end = end->next;
}


SyntaxNode* Statements::get(unsigned int index) const
{
    unsigned int i = 0;
    for (Statement* node = start; node != nullptr; node = node->next)
    {
        if (i == index)
        {
            return node->root;
        }

        i++;
    }
}


SyntaxNode* Statements::getFirst() const
{
    return start->root;
}


SyntaxNode* Statements::getLast() const
{
    return end->root;
}


