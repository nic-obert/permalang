#include "syntax_tree.hh"


using namespace syntax_tree;


void SyntaxTree::compileBlock(Tokens::Token* root)
{
    using namespace Tokens;

    Token** children = (Token**) root->value;

    for (unsigned char i = 0; i != (unsigned char) operatorType(root->opCode); i++)
    {
        Token* operand = children[i];
        
        if (isOperator(operand->opCode) && operand->opCode != OpCodes::PUSH_SCOPE)
        {
            tac.parseOperator(operand);
        }
    }

}

