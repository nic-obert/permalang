#include "syntax_tree.hh"
#include "pvm.hh"
#include "errors.hh"


#define hasReturnValueInRegister(token) (token->priority == 1)

#define setReturnValueToRegister(token) token->priority = 1

// extracts the std::string* identifier from a Token
#define IdOf(token) ((std::string*) token->value)

#define DO_STORE_RESULT true
#define DONT_STORE_RESULT false


using namespace syntax_tree;


static inline void deleteOperands(Tokens::Token** ops, OpType opType)
{
    if (opType == OpType::STANDALONE)
    {
        return;
    }
    
    // if not STANDALONE, token is at least a UNARY operator
    delete ops[0];

    if (opType == OpType::BINARY)
    {
        // if it's BINARY, delete also the second operand
        delete ops[1];
    }
}


// lookup table for system interrupts
static const pvm::OpCode systemInterrupts[] =
{
    pvm::OpCode::PRINT,
};


static unsigned int symbolNumber = 0;

static std::string* getTmpSymbolName()
{
    // 10 is the number of numeric characters in ASCII
    // +1 is to round the result up in integer division
    const unsigned char length = (unsigned char) (symbolNumber / 10 + 1);
    std::string* string = new std::string(length, '0');

    unsigned int n = symbolNumber;

    for (unsigned char i = 0; i != length; i++)
    {
        (*string)[i] = (char) (n % 10 + '0');
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
    if (
        opType == OpType::UNARY
        || opType == OpType::STANDALONE
        || isAssignmentOp(token->opCode)
        || isFlowOp(token->opCode)
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

    // delete operands since they won't be accessed anymore after their compilation
    delete[] operands;

}


static void byteCodeForUnaryOperation(Tokens::Token** operands, pvm::OpCode opCode, pvm::ByteList& byteList)
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
        byteList.add(new ByteNode(SymbolTable::get(IdOf(operands[0]))->stackPosition));
    }
    else
    {
        byteList.add(new ByteNode(OpCode::LD_CONST_A));
        byteList.add(new ByteNode(operands[0]->value)); 
    }

    if (opCode != OpCode::NO_OP)
    {
        byteList.add(new ByteNode(opCode));
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
        byteList.add(new ByteNode(SymbolTable::get(IdOf(operands[0]))->stackPosition));
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
        byteList.add(new ByteNode(SymbolTable::get(IdOf(operands[1]))->stackPosition));
    }
    else
    {
        byteList.add(new ByteNode(OpCode::LD_CONST_B));
        byteList.add(new ByteNode(operands[1]->value)); 
    }

    if (opCode != OpCode::NO_OP)
    {
        byteList.add(new ByteNode(opCode));
    }

}


static std::string* storeLiteral(Value value, Tokens::TokenType type, pvm::ByteList& byteList)
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
        new Symbol(0, type)
    );

    byteList.add(new ByteNode(OpCode::MEM_SET));
    byteList.add(new ByteNode(SymbolTable::get(name)->stackPosition));
    byteList.add(new ByteNode(value));

    return name;
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
        new Symbol(0, type)
    );

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
            byteList.add(new ByteNode(SymbolTable::get(IdOf(operands[1]))->stackPosition));
        }
        else // token is a literal
        {
            byteList.add(new ByteNode(OpCode::MEM_SET));
            byteList.add(new ByteNode(lValue->stackPosition));
            // pass as second operand the token's literal value
            byteList.add(new ByteNode(operands[1]->value));
        }

        // these operands won't be accessed anymore after compilation
        deleteOperands(operands, OpType::BINARY);
        
        return lValue->stackPosition;
    }


    case OpCodes::ARITHMETICAL_SUM:
    {
        byteCodeForBinaryOperation(operands, OpCode::ADD, byteList);

        deleteOperands(operands, OpType::BINARY);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, TokenType::LONG, byteList);
        }
        
        setReturnValueToRegister(token);

        // sum operations do not return any stack pointer
        // since the result is stored in the result register
        return 0;
    }


    case OpCodes::ARITHMETICAL_INC:
    {

        Token one = Token(TokenType::INT, 0, OpCodes::LITERAL);
        Token* ops[2] = {operands[0], &one};

        byteCodeForBinaryOperation(ops, OpCode::ADD, byteList);

        deleteOperands(operands, OpType::UNARY);
        
        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, TokenType::LONG, byteList);
        }
        
        setReturnValueToRegister(token);
        
        return 0;
    }


    case OpCodes::ARITHMETICAL_DEC:
    {

        Token one = Token(TokenType::INT, 0, OpCodes::LITERAL);
        Token* ops[2] = {operands[0], &one};

        byteCodeForBinaryOperation(ops, OpCode::SUB, byteList);

        deleteOperands(operands, OpType::UNARY);
        
        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, TokenType::LONG, byteList);
        }
        
        setReturnValueToRegister(token);
        
        return 0;
    }


    case OpCodes::ARITHMETICAL_SUB:
    {
        byteCodeForBinaryOperation(operands, OpCode::SUB, byteList);

        deleteOperands(operands, OpType::BINARY);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, TokenType::LONG, byteList);
        }
        
        setReturnValueToRegister(token);
        
        return 0;
    }


    case OpCodes::ARITHMETICAL_MUL:
    {
        byteCodeForBinaryOperation(operands, OpCode::MUL, byteList);

        deleteOperands(operands, OpType::BINARY);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, TokenType::LONG, byteList);
        }
        
        setReturnValueToRegister(token);

        return 0;
    }


    case OpCodes::ARITHMETICAL_DIV:
    {
        byteCodeForBinaryOperation(operands, OpCode::DIV, byteList);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, TokenType::LONG, byteList);
        }
        
        setReturnValueToRegister(token);
        
        return 0;
    }


    case OpCodes::LOGICAL_EQ:
    {
        byteCodeForBinaryOperation(operands, OpCode::CMP, byteList);

        deleteOperands(operands, OpType::BINARY);

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

        deleteOperands(operands, OpType::BINARY);

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

        deleteOperands(operands, OpType::BINARY);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::ZERO_FLAG, TokenType::BOOL, byteList);
        }
        
        setReturnValueToRegister(token);

        return 0;
    }


    case OpCodes::LOGICAL_LESS_EQ:
    {
        /*
            a - b
            if zero flag jump @l1
            zero flag = sign flag
        @l1:
        */

        byteCodeForBinaryOperation(operands, OpCode::SUB, byteList);

        byteList.add(new ByteNode(OpCode::IF_JUMP));
        byteList.add(new ByteNode(SymbolTable::getStackPointer() + 3));

        byteList.add(new ByteNode(OpCode::REG_TO_REG));
        byteList.add(new ByteNode(Registers::ZERO_FLAG));
        byteList.add(new ByteNode(Registers::SIGN_FLAG));

        deleteOperands(operands, OpType::BINARY);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::ZERO_FLAG, TokenType::BOOL, byteList);
        }

        setReturnValueToRegister(token);

        return 0;

    }

    
    case OpCodes::LOGICAL_GREATER_EQ:
    {
        /*
            b - a
            if zero flag jump @l1
            zero flag = sign flag
        @l1:
        */

        Token* ops[2] = {operands[1], operands[0]};

        byteCodeForBinaryOperation(ops, OpCode::SUB, byteList);

        byteList.add(new ByteNode(OpCode::IF_JUMP));
        byteList.add(new ByteNode(SymbolTable::getStackPointer() + 3));

        byteList.add(new ByteNode(OpCode::REG_TO_REG));
        byteList.add(new ByteNode(Registers::ZERO_FLAG));
        byteList.add(new ByteNode(Registers::SIGN_FLAG));

        deleteOperands(operands, OpType::BINARY);

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

        deleteOperands(operands, OpType::BINARY);

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

        deleteOperands(operands, OpType::UNARY);

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

        deleteOperands(operands, OpType::BINARY);

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

        deleteOperands(operands, OpType::BINARY);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::ZERO_FLAG, TokenType::BOOL, byteList);
        }
        
        setReturnValueToRegister(token);

        return 0;
    }


    case OpCodes::SYSTEM:
    {
        OpCode code = systemInterrupts[operands[0]->value];

        byteList.add(new ByteNode(code));

        deleteOperands(operands, OpType::UNARY);

        return 0;
    }


    case OpCodes::SYSTEM_LOAD:
    {
        byteCodeForUnaryOperation(operands, OpCode::NO_OP, byteList);

        deleteOperands(operands, OpType::UNARY);

        return 0;
    }


    case OpCodes::PARENTHESIS:
    {

        if (doStoreResult)
        {
            if (hasReturnValueInRegister(operands[0]))
            {
                // copy return value from register to memory
                return (size_t) storeResult(Registers::RESULT, tokenTypeOf(operands[0]), byteList);
            }

            if (token->opCode == OpCodes::REFERENCE)
            {
                return SymbolTable::get(IdOf(operands[0]))->stackPosition;
            }

            // token is a literal value
            
            return (size_t) storeLiteral(operands[0]->value, tokenTypeOf(operands[0]), byteList);
        }

        // if not storeResult:

        TokenType contentType = tokenTypeOf(operands[0]);

        if (operands[0]->opCode == OpCodes::REFERENCE)
        {
            // if parenthesis content is a reference, load it into the correct register and return 0

            if (contentType == TokenType::BOOL)
            {
                // if bool --> load result into a bit register (ZERO FLAG)
                byteList.add(new ByteNode(OpCode::LD_ZERO_FLAG));
                byteList.add(new ByteNode(SymbolTable::get(IdOf(operands[0]))->stackPosition));
            }
            else
            {   
                // treat token as a long
                byteList.add(new ByteNode(OpCode::LD_RESULT));
                byteList.add(new ByteNode(SymbolTable::get(IdOf(operands[0]))->stackPosition));
            }

            // to have the content's result readily available for the next operator
            setReturnValueToRegister(token);
            return 0;
        }
        
        // if token is a literal value --> move its properties to this token (parenthesis)
        
        // use a temporary variable to avoid accessing freed memory (for safety resons)
        Value tmpValue = operands[0]->value;

        token->type = operands[0]->type;
        token->opCode = OpCodes::LITERAL;

        deleteOperands(operands, OpType::UNARY);

        // token->value will be set to tmpValue by the caller function
        return tmpValue;
    }


    case OpCodes::FLOW_IF:
    {
        /*
            - evaluate boolean condition
            - conditional jump based on condition
            - if body block
        */

        parseTokenOperator(operands[0]);

        byteList.add(new ByteNode(OpCode::IF_JUMP));
        // bodySizeNode will be set later when the actual if body is compiled
        ByteNode* bodySizeNode = new ByteNode(SymbolTable::getStackPointer());
        byteList.add(bodySizeNode);

        parseTokenOperator(operands[1]);

        // update the number of bytes to jump over (size of the if's body)
        bodySizeNode->data += SymbolTable::getStackPointer() - bodySizeNode->data;

        deleteOperands(operands, OpType::BINARY);

        return 0;
    }


    } // switch(token->opCode)


    // in case token->opCodes is unhandled

    std::string number;
    string_utils::byteToString((unsigned char) token->opCode, number);

    std::string msg("Unhandled OpCodes in byte code generation: ");
    
    msg += number;
    errors::UnexpectedBehaviourError(msg);
    return 0;
}

