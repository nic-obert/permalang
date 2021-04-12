#include "tac.hh"
#include "utils.hh"
#include "op_codes.hh"


// converts a token->value (real reference) to a const Address*
#define toAddress(reference) ((const Address*) reference)

#define isReference(token) (token->opCode == OpCodes::REFERENCE)


using namespace tac;



void Tac::parseTree(syntax_tree::SyntaxTree& tree)
{
    using namespace syntax_tree;
    using namespace Tokens;

    for (Statement* statement = tree.statements.start; statement != nullptr; statement = statement->next)
    {
        Token* root = statement->root;

        if (isOperator(root->opCode))
        {
            parseOperator(root);
        }

    }

}


void Tac::parseOperator(Tokens::Token* token)
{
    using namespace Tokens;

    // treat token's value as a pointer to an array of token pointers
    Token** operands = (Token**) token->value; 
    
    // loop over operands and evaluate those first
    for (unsigned char i = 0; i != operatorType(token->opCode); i++)
    {
        Token* operand = operands[i];

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
                new TacValue(isReference(operands[0]), operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            token->opCode = REFERENCE;
            token->value = toValue(result);

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
                new TacValue(isReference(operands[0]), operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            token->opCode = REFERENCE;
            token->value = toValue(result);

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
                new TacValue(isReference(operands[0]), operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            token->opCode = REFERENCE;
            token->value = toValue(result);

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
                new TacValue(isReference(operands[0]), operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            token->opCode = REFERENCE;
            token->value = toValue(result);

            break;
        }

        case OpCodes::ASSIGNMENT_ASSIGN:
        {
            /*
                a = b
            */

            add(new TacInstruction(
                TacOp::ASSIGN,
                new TacValue(true, operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            token->value = operands[0]->value;

            break;
        }

        case OpCodes::ASSIGNMENT_ADD:
        {
            /*
                a = a + b
            */

            add(new TacInstruction(
                TacOp::SUM,
                new TacValue(true, operands[0]->value),
                new TacValue(true, operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            break;
        }

        case OpCodes::ASSIGNMENT_SUB:
        {
            /*
                a = a - b
            */

            add(new TacInstruction(
                TacOp::SUB,
                new TacValue(true, operands[0]->value),
                new TacValue(true, operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            break;
        }

        case OpCodes::ASSIGNMENT_MUL:
        {
            /*
                a = a * b
            */

            add(new TacInstruction(
                TacOp::MUL,
                new TacValue(true, operands[0]->value),
                new TacValue(true, operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            break;
        }

        case OpCodes::ASSIGNMENT_DIV:
        {
            /*
                a = a / b
            */

            add(new TacInstruction(
                TacOp::DIV,
                new TacValue(true, operands[0]->value),
                new TacValue(true, operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            break;
        }

        case OpCodes::ARITHMETICAL_INC:
        {
            /*
                a = a + 1
            */

            add(new TacInstruction(
                TacOp::SUM,
                new TacValue(true, operands[0]->value),
                new TacValue(true, operands[0]->value),
                new TacValue(false, 1)
            ));

            token->value = operands[0]->value;

            break;
        }

        case OpCodes::ARITHMETICAL_DEC:
        {
            /*
                a = a - 1
            */

            add(new TacInstruction(
                TacOp::SUB,
                new TacValue(true, operands[0]->value),
                new TacValue(true, operands[0]->value),
                new TacValue(false, 1)
            ));

            token->value = operands[0]->value;

            break;
        }

        case OpCodes::LOGICAL_EQ:
        {
            /*
                r = a == b
            */

            const Address* result = Address::getAddress();

            add(new TacInstruction(
                TacOp::EQ,
                new TacValue(true, toValue(result)),
                new TacValue(isReference(operands[0]), operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            token->opCode = REFERENCE;
            token->value = toValue(result);
        
            break;
        }

        case OpCodes::LOGICAL_NOT_EQ:
        {
            /*
                r = a == b
                r = r == 0
            */

            const Address* result = Address::getAddress();

            add(new TacInstruction(
                TacOp::EQ,
                new TacValue(true, toValue(result)),
                new TacValue(isReference(operands[0]), operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));
            add(new TacInstruction(
                TacOp::EQ,
                new TacValue(true, toValue(result)),
                new TacValue(true, toValue(result)),
                new TacValue(false, 0)
            ));

            token->opCode = REFERENCE;
            token->value = toValue(result);

            break;
        }
    }

}