
#include "token.hh"
#include "syntax_tree.hh"


syntax_tree::SyntaxTree::SyntaxTree(Tokens::TokenList* tokens)
: tokens(tokens)
{

}


void syntax_tree::SyntaxTree::parse()
{
    while (true)
    {
        Tokens::Token* token = tokens->getHighestPriority();
        if (token->priority == 0)
        {
            break;
        }

        token->satisfy();

    }
}

