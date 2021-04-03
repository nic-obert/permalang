#include "byte_compiler.hh"
#include "syntax_tree.hh"
#include "token.hh"


using namespace byte_compiler;


void Compiler::compile(syntax_tree::SyntaxTree* syntaxTree)
{

    using namespace Tokens;
    using namespace syntax_tree;
    

    for (statement = syntaxTree->statements.start; statement != nullptr; statement = statement)
    {

        token = statement->root;
        if (token == nullptr)
        {
            continue;
        }

        
        
    }

}

