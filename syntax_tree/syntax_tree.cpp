#pragma once

#include "token.cpp"

using namespace Tokens;


namespace syntax_tree
{

    class SyntaxTree
    {
    private:

        Token* root;
        TokenList* tokens;

    public:

        SyntaxTree(TokenList* tokens)
        : tokens(tokens)
        {

        }


        void parse()
        {
            while (true)
            {
                Token* token = tokens->getHighestPriority();
            }
        }

    };

};
