#include "tac.hh"
#include "utils.hh"
#include "op_codes.hh"
#include "token.hh"


// converts a token->value (real reference) to a const Address*
#define toAddress(reference) ((const Address*) reference)

#define isReference(token) ((TacValueType) (token->opCode == OpCodes::REFERENCE))


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
    // OpType is compatible with integers 0, 1, 2 which are
    // respectively STANDALONE, UNARY, BINARY
    for (unsigned char i = 0; i != (unsigned char) operatorType(token->opCode); i++)
    {
        Token* operand = operands[i];

        if (isOperator(operand->opCode))
        {
            parseOperator(operand);
        }

    }

    // generate three address code for the operator
    token->value = toValue(tacFor(token->opCode, operands));
    token->opCode = OpCodes::REFERENCE;

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
                new TacValue(TacValueType::ADDRESS, toValue(result)),
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
                new TacValue(TacValueType::ADDRESS, toValue(result)),
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
                new TacValue(TacValueType::ADDRESS, toValue(result)),
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
                new TacValue(TacValueType::ADDRESS, toValue(result)),
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
                new TacValue(TacValueType::ADDRESS, operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            return toAddress(operands[0]->value);
        }

        case OpCodes::LOGICAL_EQ:
        {
            /*
                r = a == b
            */

            const Address* result = Address::getAddress();

            add(new TacInstruction(
                TacOp::EQ,
                new TacValue(TacValueType::ADDRESS, toValue(result)),
                new TacValue(isReference(operands[0]), operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));
        
            return result;
        }

        case OpCodes::LOGICAL_LESS:
        {
            /*
                r = a < b
            */

            const Address* result = Address::getAddress();

            add(new TacInstruction(
                TacOp::LESS,
                new TacValue(TacValueType::ADDRESS, toValue(result)),
                new TacValue(isReference(operands[0]), operands[0]->value),
                new TacValue(isReference(operands[1]), operands[1]->value)
            ));

            return result;
        }

// COMPLEX OPERATIONS


        case OpCodes::ASSIGNMENT_ADD:
        {
            /*
                a = a + b
            */

            add(new TacInstruction(
                TacOp::SUM,
                new TacValue(TacValueType::ADDRESS, operands[0]->value),
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
                new TacValue(TacValueType::ADDRESS, operands[0]->value),
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
                new TacValue(TacValueType::ADDRESS, operands[0]->value),
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
                new TacValue(TacValueType::ADDRESS, operands[0]->value),
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
                new TacValue(TacValueType::ADDRESS, operands[0]->value),
                new TacValue(TacValueType::ADDRESS, operands[0]->value),
                new TacValue(TacValueType::LITERAL, 1)
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
                new TacValue(TacValueType::ADDRESS, operands[0]->value),
                new TacValue(TacValueType::ADDRESS, operands[0]->value),
                new TacValue(TacValueType::LITERAL, 1)
            ));

            break;
        }

        case OpCodes::LOGICAL_NOT_EQ:
        {
            /*
                r = a == b
                r = r == 0
            */

            const Address* result = tacFor(OpCodes::LOGICAL_EQ, operands);

            // create an array of operands
            Token op1 = Token(TokenType::NONE, 0, OpCodes::REFERENCE, toValue(result));
            Token op2 = Token(TokenType::NONE, 0, OpCodes::LITERAL, 0);
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
                
            // create array of operands
            Token op2 = Token(TokenType::INT, 0, OpCodes::LITERAL, 1);
            Token* ops[2] = { operands[0], &op2 };

            TacInstruction* l1 = new TacInstruction(TacOp::LABEL);
            TacInstruction* l2 = new TacInstruction(TacOp::LABEL);

            const Address* result = tacFor(OpCodes::LOGICAL_EQ, ops);

            add(new TacInstruction(
                TacOp::IF,
                new TacValue(TacValueType::ADDRESS, toValue(result)),
                new TacValue(TacValueType::LABEL, toValue(l1))
            ));
            add(new TacInstruction(
                TacOp::JUMP,
                new TacValue(TacValueType::LABEL, toValue(l2))
            ));

            add(l1);

            ops[0] = operands[0];
            
            result = tacFor(OpCodes::LOGICAL_EQ, ops);

            add(l2);

            return result;
        }

        case OpCodes::LOGICAL_OR:
        {
            /*
                r = a == 1
                if r jump l2
                r = b == 1
            l1:

            */

            TacInstruction* l1 = new TacInstruction(TacOp::LABEL);

            Token op2 = Token(TokenType::INT, 0, OpCodes::LITERAL, 1);
            Token* ops[2] = { operands[0], &op2 };

            const Address* result = tacFor(OpCodes::LOGICAL_EQ, ops);

            add(new TacInstruction(
                TacOp::IF,
                new TacValue(TacValueType::ADDRESS, toValue(result)),
                new TacValue(TacValueType::LABEL, toValue(l1))
            ));

            ops[0] = operands[1];

            result = tacFor(OpCodes::LOGICAL_EQ, ops);

            add(l1);

            return result;
        }

        case OpCodes::LOGICAL_NOT:
        {
            /*
                r = a == 0
            */

            Token op2 = Token(TokenType::INT, 0, OpCodes::LITERAL, 0);
            Token* ops[2] = { operands[0], &op2 };

            return tacFor(OpCodes::LOGICAL_EQ, ops);
        }

        case OpCodes::LOGICAL_GREATER:
        {
            /*
                just flip the operands

                r = b < a
            */

            Token* ops[2] = { operands[1], operands[0] };

            return tacFor(OpCodes::LOGICAL_LESS, ops);
        }

        case OpCodes::LOGICAL_LESS_EQ:
        {
            /*
                r = a < b
                if r jump l1
                r = a == b
            l1:

            */

            TacInstruction* l1 = new TacInstruction(TacOp::LABEL);

            const Address* result = tacFor(OpCodes::LOGICAL_LESS, operands);

            add(new TacInstruction(
                TacOp::IF,
                new TacValue(TacValueType::ADDRESS, toValue(result)),
                new TacValue(TacValueType::LABEL, toValue(l1))
            ));

            result = tacFor(OpCodes::LOGICAL_EQ, operands);

            add(l1);

            return result;
        }

        case OpCodes::LOGICAL_GREATER_EQ:
        {
            /*
                r = a > b
                if r jump l1
                r = a == b
            l1:

            */

            TacInstruction* l1 = new TacInstruction(TacOp::LABEL);

            const Address* result = tacFor(OpCodes::LOGICAL_GREATER, operands);

            add(new TacInstruction(
                TacOp::IF,
                new TacValue(TacValueType::ADDRESS, toValue(result)),
                new TacValue(TacValueType::LABEL, toValue(l1))
            ));

            result = tacFor(OpCodes::LOGICAL_EQ, operands);

            add(l1);

            return result;            
        }

        case OpCodes::ADDRESS_OF:
        {
            /*
                r = [a]
            */

            const Address* result = Address::getAddress();

            add(new TacInstruction(
                TacOp::ASSIGN,
                new TacValue(TacValueType::ADDRESS, toValue(result)),
                new TacValue(TacValueType::LITERAL, operands[0]->value)
            ));

            return result;
        }

    } // switch (token->opCode)

}