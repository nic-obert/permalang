#include "syntax_tree.hh"
#include "pvm.hh"


#define hasReturnValueInRegister(token) (token->priority == 1)

#define setReturnValueToRegister(token) token->priority = 1

#define DO_STORE_RESULT true
#define DONT_STORE_RESULT false


using namespace syntax_tree;


static unsigned int symbolNumber = 0;

static std::string* getTmpSymbolName()
{
    // 10 is the number of numeric characters in ASCII
    // +1 is to round the result up in integer division
    const unsigned char length = symbolNumber / 10 + 1;
    std::string* string = new std::string(length, '0');

    unsigned int n = symbolNumber;

    for (unsigned char i = 0; i != length; i++)
    {
        (*string)[i] = n % 10 + '0';
        // integer division discards rightmost digit (e.g. 325 -> 32)
        n /= 10;
    }

    symbolNumber ++;
    
    return string;
}


void SyntaxTree::parseTokenOperator(Tokens::Token* token)
{
    using namespace Tokens;

    // treat token's value as a pointer to an array of token pointers
    Token** operands = (Token**) token->value; 
    OpType opType = operatorType(token->opCode);
    
    // control flow statements parse themselves their own operands
    if (!isFlowOp(token->opCode))
    {
        /*
            loop over operands and evaluate those first
            OpType is compatible with integers 0, 1, 2 which are
            respectively STANDALONE, UNARY, BINARY
            
            unsigned char is used because is the smallest data type (1 byte)
        */
        for (unsigned char i = 0; i != (unsigned char) opType; i++)
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

    // if token is a unary operator the result stored in the result register of an
    // eventual operation performed in the evaluation of its operand will not be overwritten 
    if (opType == OpType::UNARY ||
        opType == OpType::STANDALONE ||
        isAssignmentOp(token->opCode)
        )
    {
        token->value = toValue(byteCodeFor(token, operands, DONT_STORE_RESULT));
    }
    else
    {
        token->value = toValue(byteCodeFor(token, operands, DO_STORE_RESULT));
        // token is now a reference to its operation's result
        token->opCode = OpCodes::REFERENCE;
    }

}


// operation size: 19 bytes
static void byteCodeForBinaryOperation(Tokens::Token** operands, pvm::OpCode opCode, pvm::ByteList& byteList)
{
    using namespace pvm;
    using namespace symbol_table;

    if (hasReturnValueInRegister(operands[0]))
    {
        byteList.add(new ByteNode(OpCode::REG_MOV));
        byteList.add(new ByteNode(Registers::GENERAL_A));
        byteList.add(new ByteNode(Registers::RESULT));
    }
    else if (operands[0]->opCode == OpCodes::REFERENCE)
    {
        
        byteList.add(new ByteNode(OpCode::LD_A));
        byteList.add(new ByteNode(SymbolTable::get((std::string*) operands[0]->value)->stackPosition));
    }
    else
    {
        byteList.add(new ByteNode(OpCode::LD_CONST_A));
        byteList.add(new ByteNode(operands[0]->value)); 
    }

    if (hasReturnValueInRegister(operands[1]))
    {
        byteList.add(new ByteNode(OpCode::REG_MOV));
        byteList.add(new ByteNode(Registers::GENERAL_B));
        byteList.add(new ByteNode(Registers::RESULT));
    }
    else if (operands[1]->opCode == OpCodes::REFERENCE)
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


static std::string* storeResult(pvm::Registers reg, Tokens::TokenType type, pvm::ByteList& byteList)
{
    using namespace symbol_table;
    using namespace Tokens;
    using namespace pvm;

    // declare a new Symbol holding the value
    // push the operation result onto the stack

    // TODO string pointer should be deleted later

    std::string* name = getTmpSymbolName();

    SymbolTable::declare(
        name,
        new Symbol(0, TokenType::DOUBLE));

    byteList.add(new ByteNode(OpCode::REG_MOV));
    byteList.add(new ByteNode(SymbolTable::get(name)->stackPosition));
    byteList.add(new ByteNode(reg));

    return name;
}


size_t SyntaxTree::byteCodeFor(Tokens::Token* token, Tokens::Token** operands, bool doStoreResult)
{
    using namespace Tokens;
    using namespace pvm;
    using namespace symbol_table;

    switch(token->opCode)
    {

    case OpCodes::ASSIGNMENT_ASSIGN:
    {
        Symbol* lValue = SymbolTable::get((std::string*) operands[0]->value);

        if (hasReturnValueInRegister(operands[1]))
        {
            byteList.add(new ByteNode(OpCode::REG_MOV));
            byteList.add(new ByteNode(lValue->stackPosition));
            byteList.add(new ByteNode(Registers::RESULT));
        }
        else if (operands[1]->opCode == OpCodes::REFERENCE)
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

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, TokenType::DOUBLE, byteList);
        }
        
        setReturnValueToRegister(token);

        // sum operations do not return any stack pointer
        // since the result is stored in the result register
        return 0;
    }


    case OpCodes::ARITHMETICAL_SUB:
    {
        byteCodeForBinaryOperation(operands, OpCode::SUB, byteList);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, TokenType::DOUBLE, byteList);
        }
        
        setReturnValueToRegister(token);
        
        return 0;
    }


    case OpCodes::ARITHMETICAL_MUL:
    {
        byteCodeForBinaryOperation(operands, OpCode::MUL, byteList);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, TokenType::DOUBLE, byteList);
        }
        
        setReturnValueToRegister(token);

        return 0;
    }


    case OpCodes::ARITHMETICAL_DIV:
    {
        byteCodeForBinaryOperation(operands, OpCode::DIV, byteList);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, TokenType::DOUBLE, byteList);
        }
        
        setReturnValueToRegister(token);
        
        return 0;
    }


    case OpCodes::LOGICAL_EQ:
    {
        byteCodeForBinaryOperation(operands, OpCode::CMP, byteList);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::ZERO_FLAG, TokenType::BOOL, byteList);
        }
        
        setReturnValueToRegister(token);
        
        return 0;
    }

    
    case OpCodes::LOGICAL_NOT_EQ:
    {
        byteCodeForBinaryOperation(operands, OpCode::CMP_REVERSE, byteList);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::ZERO_FLAG, TokenType::BOOL, byteList);
        }
        
        setReturnValueToRegister(token);

        return 0;
    }

    
    case OpCodes::LOGICAL_LESS:
    {
        /*
            tmp = a - b
            zero flag = tmp < 0
        */
        byteCodeForBinaryOperation(operands, OpCode::SUB, byteList);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::ZERO_FLAG, TokenType::BOOL, byteList);
        }
        
        setReturnValueToRegister(token);

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

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::ZERO_FLAG, TokenType::BOOL, byteList);
        }
        
        setReturnValueToRegister(token);

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

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::ZERO_FLAG, TokenType::BOOL, byteList);
        }
        
        setReturnValueToRegister(token);

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

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::ZERO_FLAG, TokenType::BOOL, byteList);
        }
        
        setReturnValueToRegister(token);

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

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::ZERO_FLAG, TokenType::BOOL, byteList);
        }
        
        setReturnValueToRegister(token);

        return 0;
    }

    } // switch(token->opCode)


    std::cerr << "unhandled opcodes: " << token->opCode << std::endl;
    exit(1);

}

