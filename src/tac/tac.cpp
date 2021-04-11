#include "tac.hh"
#include "token.hh"
#include "syntax_tree.hh"
#include "op_codes.hh"
#include "utils.hh"


// converts a token->value (real reference) to a const Address*
#define toAddress(reference) ((const Address*) reference)

#define isReference(opCode) (opCode == OpCodes::REFERENCE)


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
    
    // loop over operands and evaluate those first
    for (unsigned char i = 0; i != operatorType(token->opCode); i++)
    {
        const Token* operand = operands[i];

        if (isOperator(operand->opCode))
        {
            parseOperator(operand);
        }

    }

    // generate three address code for the operator
    switch (token->opCode)
    {
        case OpCodes::ARITHMETICAL_SUM:
        {
            /*
                r = a + b
            */

            const Address* result = Address::getAddress();

            add(new TacInstruction(
                TacOp::SUM,
                new TacValue(true, toValue(result)),
                new TacValue(isReference(operands[0]->opCode), operands[0]->value),
                new TacValue(isReference(operands[1]->opCode), operands[1]->value)
            ));

            break;
        }

        case OpCodes::ARITHMETICAL_SUB:
        {
            /*
                r = a - b
            */

            const Address* result = Address::getAddress();

            add(new TacInstruction(
                TacOp::SUB,
                new TacValue(true, toValue(result)),
                new TacValue(isReference(operands[0]->opCode), operands[0]->value),
                new TacValue(isReference(operands[1]->opCode), operands[1]->value)
            ));

            break;
        }

        case OpCodes::ARITHMETICAL_MUL:
        {
            /*
                r = a * b
            */

            const Address* result = Address::getAddress();

            add(new TacInstruction(
                TacOp::MUL,
                new TacValue(true, toValue(result)),
                new TacValue(isReference(operands[0]->opCode), operands[0]->value),
                new TacValue(isReference(operands[1]->opCode), operands[1]->value)
            ));

            break;
        }

        case OpCodes::ARITHMETICAL_DIV:
        {
            /*
                r = a - b
            */

            const Address* result = Address::getAddress();

            add(new TacInstruction(
                TacOp::DIV,
                new TacValue(true, toValue(result)),
                new TacValue(isReference(operands[0]->opCode), operands[0]->value),
                new TacValue(isReference(operands[1]->opCode), operands[1]->value)
            ));

            break;
        }

        case OpCodes::ASSIGNMENT_ASSIGN:
        {
            /*
                a = b
            */

            add(new TacInstruction(
                TacOp::ASSIGN,
                new TacValue(true, toValue(operands[0]->value)),
                new TacValue(isReference(operands[1]->opCode), toValue(operands[1]->value))
            ));

            break;
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

