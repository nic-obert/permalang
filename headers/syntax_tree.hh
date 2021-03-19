
#include "token.hh"


namespace syntax_tree
{   

    class SyntaxTree
    {
    private:

        Tokens::Token* root;
        Tokens::TokenList* tokens;

    public:

        SyntaxTree(Tokens::TokenList* tokens);
       

        void parse();

    };

};
