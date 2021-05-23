#include "syntax_tree.hh"
#include "pvm.hh"


using namespace syntax_tree;


void SyntaxTree::parseTokenOperator(Tokens::Token* token)
{
    using namespace Tokens;

    // treat token's value as a pointer to an array of token pointers
    Token** operands = (Token**) token->value; 
    
    // control flow statements parse themselves their own operands
    if (!isFlowOp(token->opCode))
    {
        /*
            loop over operands and evaluate those first
            OpType is compatible with integers 0, 1, 2 which are
            respectively STANDALONE, UNARY, BINARY
            
            unsigned char is used because is the smallest data type (1 byte)
        */
        for (unsigned char i = 0; i != (unsigned char) operatorType(token->opCode); i++)
        {
            Token* operand = operands[i];

            /*
                do not parse an operand if it's not an operator itself
                or if it's a scope, since it will be parsed by 
                the operator the scope is required by
            */
            if (isOperator(operand->opCode) && operand->opCode != OpCodes::PUSH_SCOPE)
            {
                parseTokenOperator(operand);
            }

        }

    } // if (isFLowOp(token->opCode))


    token->value = toValue(byteCodeFor(token, operands));
    // token is now a reference to its operation's result
    token->opCode = OpCodes::REFERENCE;

}


// operation size: 19 bytes
static void byteCodeForBinaryOperation(Tokens::Token** operands, pvm::OpCode opCode, pvm::ByteList byteList)
{
    using namespace pvm;
    using namespace symbol_table;

    if (operands[0]->opCode == OpCodes::REFERENCE)
    {
        
        byteList.add(new ByteNode(OpCode::LD_A));
        byteList.add(new ByteNode(SymbolTable::get((std::string*) operands[0]->value)->stackPosition));
    }
    else
    {
        byteList.add(new ByteNode(OpCode::LD_CONST_A));
        byteList.add(new ByteNode(operands[0]->value)); 
    }

    if (operands[1]->opCode == OpCodes::REFERENCE)
    {
        
        byteList.add(new ByteNode(OpCode::LD_B));
        byteList.add(new ByteNode(SymbolTable::get((std::string*) operands[1]->value)->stackPosition));
    }
    else
    {
        byteList.add(new ByteNode(OpCode::LD_CONST_B));
        byteList.add(new ByteNode(operands[1]->value)); 
    }

    byteList.add(new ByteNode(opCode));

}


size_t SyntaxTree::byteCodeFor(Tokens::Token* token, Tokens::Token** operands)
{
    using namespace Tokens;
    using namespace pvm;
    using namespace symbol_table;

    switch(token->opCode)
    {

    case OpCodes::ASSIGNMENT_ASSIGN:
    {
        Symbol* lValue = SymbolTable::get((std::string*) operands[0]->value);

        // if second operand is a reference, get its stack position from the symbol table
        if (operands[1]->opCode == OpCodes::REFERENCE)
        {
            byteList.add(new ByteNode(OpCode::MEM_MOV));
            byteList.add(new ByteNode(lValue->stackPosition));
            byteList.add(new ByteNode(SymbolTable::get((std::string*) operands[1]->value)->stackPosition));
        }
        else // token is a literal
        {
            byteList.add(new ByteNode(OpCode::MEM_SET));
            byteList.add(new ByteNode(lValue->stackPosition));
            // pass as second operand the token's literal value
            byteList.add(new ByteNode(operands[1]->value));
        }
        
        return lValue->stackPosition;
    }


    case OpCodes::ARITHMETICAL_SUM:
    {
        byteCodeForBinaryOperation(operands, OpCode::ADD, byteList);

        // sum operations do not return any stack pointer
        // since the result is stored in the result register
        return 0;
    }


    case OpCodes::ARITHMETICAL_SUB:
    {
        byteCodeForBinaryOperation(operands, OpCode::SUB, byteList);
        return 0;
    }


    case OpCodes::ARITHMETICAL_MUL:
    {
        byteCodeForBinaryOperation(operands, OpCode::MUL, byteList);
        return 0;
    }


    case OpCodes::ARITHMETICAL_DIV:
    {
        byteCodeForBinaryOperation(operands, OpCode::DIV, byteList);
        return 0;
    }


    case OpCodes::LOGICAL_EQ:
    {
        byteCodeForBinaryOperation(operands, OpCode::CMP, byteList);
        return 0;
    }

    
    case OpCodes::LOGICAL_NOT_EQ:
    {
        byteCodeForBinaryOperation(operands, OpCode::CMP_REVERSE, byteList);
        return 0;
    }

    
    case OpCodes::LOGICAL_LESS:
    {
        /*
            tmp = a - b
            zero flag = tmp < 0
        */
        byteCodeForBinaryOperation(operands, OpCode::SUB, byteList);
        return 0;
    }


    case OpCodes::LOGICAL_GREATER:
    {
        /*
            tmp = b - a
            zero flag = tmp < 0
        */

        // just invert operands
        Token* ops[2] = {operands[1], operands[0]};

        byteCodeForBinaryOperation(ops, OpCode::SUB, byteList);
        return 0;
    }


    case OpCodes::LOGICAL_NOT:
    {
        /*
            zero flag = a == 0
        */

        Token zero = Token(TokenType::INT, 0, OpCodes::LITERAL, 0);
        Token* ops[2] = {operands[0], &zero};

        byteCodeForBinaryOperation(ops, OpCode::CMP, byteList);
        return 0;
    }
    

    case OpCodes::LOGICAL_AND:
    {
        /*
            zero flag = a != 1
            if zero flag jump @l1
            zero flag = b != 0
        @l1:
        */

        Token one = Token(TokenType::INT, 0, OpCodes::LITERAL, 1);
        Token* ops[2] = {operands[0], &one};

        byteCodeForBinaryOperation(ops, OpCode::CMP_REVERSE, byteList);

        byteList.add(new ByteNode(OpCode::IF_JUMP));
        // address to jump to = current stack pointer + size of instructions to jump over
        byteList.add(new ByteNode(SymbolTable::getStackPointer() + 19));

        Token zero = Token(TokenType::INT, 0, OpCodes::LITERAL, 1);
        ops[0] = operands[1];
        ops[1] = &zero;

        byteCodeForBinaryOperation(ops, OpCode::CMP_REVERSE, byteList);

        return 0;
    }


    case OpCodes::LOGICAL_OR:
    {
        /*
            zero flag = a != 0
            if zero flag jump @l1
            zero flag = b != 0
        @l1:
        */

        Token zero = Token(TokenType::INT, 0, OpCodes::LITERAL, 0);
        Token* ops[2] = {operands[0], &zero};

        byteCodeForBinaryOperation(ops, OpCode::CMP_REVERSE, byteList);

        byteList.add(new ByteNode(OpCode::IF_JUMP));
        // address to jump to = current stack pointer + size of instructions to jump over
        byteList.add(new ByteNode(SymbolTable::getStackPointer() + 19));

        ops[0] = operands[1];

        byteCodeForBinaryOperation(ops, OpCode::CMP_REVERSE, byteList);

        return 0;
    }

    } // switch(token->opCode)


}

