#pragma once

#include "syntax_tree.hh"
#include "token.hh"


namespace byte_compiler
{

    class Compiler
    {
    private:

        Tokens::Token* token;
        syntax_tree::Statement* statement;


    public:

        Compiler();

        void compile(syntax_tree::SyntaxTree* syntaxTree);

    };

};
