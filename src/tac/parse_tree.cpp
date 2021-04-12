#include "tac.hh"
#include "utils.hh"
#include "op_codes.hh"
#include "token.hh"


// converts a token->value (real reference) to a const Address*
#define toAddress(reference) ((const Address*) reference)

#define isReference(token) (token->opCode == OpCodes::REFERENCE)


using namespace tac;
using namespace Tokens;



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
    token->value = toValue(tacFor(token->opCode, operands));
    token->opCode = REFERENCE;

}


const Address* Tac::tacFor(OpCodes opCode, Token** operands)
{
    switch (opCode)
    {   


// BASIC OPERATIONS


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

            return result;
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

            return result;
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

            return result;
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

            return result;
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

            return toAddress(operands[0]->value);
        }


// COMPLEX OPERATIONS


        case OpCodes::ASSIGNMENT_ADD:
        {
            /*
                a = a + b
            */

            add(new TacInstruction(
                TacOp::SUM,
                new TacValue(true, operands[0]->value),
                new TacValue(isReference(operands[0]), operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            return toAddress(operands[0]->value);
        }

        case OpCodes::ASSIGNMENT_SUB:
        {
            /*
                a = a - b
            */

            add(new TacInstruction(
                TacOp::SUB,
                new TacValue(true, operands[0]->value),
                new TacValue(isReference(operands[0]), operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            return toAddress(operands[0]->value);
        }

        case OpCodes::ASSIGNMENT_MUL:
        {
            /*
                a = a * b
            */

            add(new TacInstruction(
                TacOp::MUL,
                new TacValue(true, operands[0]->value),
                new TacValue(isReference(operands[0]), operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            return toAddress(operands[0]->value);
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
                new TacValue(isReference(operands[0]), operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            return toAddress(operands[0]->value);

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
        
            return result;
        }

        case OpCodes::LOGICAL_NOT_EQ:
        {
            /*
                r = a == b
                r = r == 0
            */

            const Address* result = tacFor(OpCodes::LOGICAL_EQ, operands);

            // create an array of operands
            Token op1 = Token(NONE, 0, REFERENCE, toValue(result));
            Token op2 = Token(NONE, 0, LITERAL, 0);
            Token* ops[2] = { &op1, &op2 };

            return tacFor(OpCodes::LOGICAL_EQ, ops);
        }

        case OpCodes::LOGICAL_AND:
        {
            /*
                r = a == 1
                if r jump l1:  
                jump l2         # first is false --> whole statement is false
            l1:
                r = b == 1
            l2:

            */
                
            add(new TacInstruction(
                TacOp::EQ
            ));


            break;
        }

    } // switch (token->opCode)

}