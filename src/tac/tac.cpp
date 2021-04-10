#include "tac.hh"
#include "token.hh"
#include "syntax_tree.hh"
#include "op_codes.hh"


using namespace tac;


Tac::Tac()
{
    // initialize with the zeroth instruction
    start = new TacInstruction(TacOp::NO_OP);
    instructions = start;
}


void Tac::add(TacInstruction* in)
{
    instructions->next = in;
    in->prev = instructions;
    instructions = in;
}


void Tac::parseTree(syntax_tree::SyntaxTree const& tree)
{
    using namespace syntax_tree;
    using namespace Tokens;

    for (const Statement* statement = tree.statements.start; statement != nullptr; statement = statement->next)
    {
        Token* root = statement->root;

        if (isOperator(root->opCode))
        {
            parseOperator(root);
        }

    }

}


void Tac::parseOperator(const Tokens::Token* token)
{
    using namespace Tokens;

    // treat token's value as a pointer to an array of token pointers
    const Token** operands = (const Token**) token->value; 
    
    // loop over operands
    for (unsigned char i = 0; i != operatorType(token->opCode); i++)
    {
        const Token* operand = operands[i];

        if (isOperator(operand->opCode))
        {
            parseOperator(operand);
        }


        switch (token->opCode)
        {

        }

    }


}


std::ostream& operator<<(std::ostream& stream, tac::Tac const& tac)
{
    stream << "Three Address Code: {\n";

    for (const TacInstruction* instruction = tac.start; instruction != nullptr; instruction = instruction->next)
    {
        stream << "\t" << *instruction << "\n";
    }

    return stream << "}";
}

