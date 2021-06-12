#include "syntax_tree.hh"
#include "pvm.hh"
#include "errors.hh"


using namespace syntax_tree;
using namespace Tokens;
using namespace pvm;


#define hasReturnValueInRegister(token) (token->priority == 1)

#define setReturnValueToRegister(token) token->priority = 1


// extracts the std::string* identifier from a Token
#define IdOf(token) ((std::string*) token->value)


#define AddNode(...) byteList.add(new ByteNode(__VA_ARGS__))


static inline size_t StackPositionOf(const Token* token)
{
    return symbol_table::SymbolTable::get(IdOf(token))->stackPosition;
}


static void addConstLoad(Registers reg, Value value, ByteList& byteList)
{
    // TODO implement also negative values
    switch (typeOfValue(value))
    {
    case TokenType::BOOL:
    
        switch (reg)
        {
            case Registers::GENERAL_A:
                AddNode(OpCode::LD_CONST_A_BIT);
                AddNode(value, 1);
                break;
            
            case Registers::GENERAL_B:
                AddNode(OpCode::LD_CONST_B_BIT);
                AddNode(value, 1);
                break;
            
            case Registers::RESULT:
                AddNode(OpCode::LD_CONST_RESULT_BIT);
                AddNode(value, 1);
                break;
        }
        break;
    
    case TokenType::BYTE:
        switch (reg)
        {
            case Registers::GENERAL_A:
                AddNode(OpCode::LD_CONST_A_1);
                AddNode(value, 1);
                break;
            
            case Registers::GENERAL_B:
                AddNode(OpCode::LD_CONST_B_1);
                AddNode(value, 1);
                break;
            
            case Registers::RESULT:
                AddNode(OpCode::LD_CONST_RESULT_1);
                AddNode(value, 1);
                break;
        }
        break;
    
    case TokenType::INT:
        switch (reg)
        {
            case Registers::GENERAL_A:
                AddNode(OpCode::LD_CONST_A_4);
                AddNode(value, 4);
                break;
            
            case Registers::GENERAL_B:
                AddNode(OpCode::LD_CONST_B_4);
                AddNode(value, 4);
                break;
            
            case Registers::RESULT:
                 AddNode(OpCode::LD_CONST_RESULT_4);
                AddNode(value, 4);
                break;
        }
        break;

    case TokenType::LONG:
        switch (reg)
        {
        case Registers::GENERAL_A:
            AddNode(OpCode::LD_CONST_A_8);
            AddNode(value, 8);
            break;
        
        case Registers::GENERAL_B:
            AddNode(OpCode::LD_CONST_B_8);
            AddNode(value, 8);
            break;
        
        case Registers::RESULT:
             AddNode(OpCode::LD_CONST_RESULT_8);
            AddNode(value, 8);
            break;
        }
        break;

    default:
    {
        // there shouldn't be other literal types or registers
        std::string msg = "Unexpected TokenType or Registers values in function static void addConstLoad(Registers reg, Value value, ByteList& byteList):\n";
        msg += "reg=" + std::string(registerName(reg)) + ", value=" + std::to_string(value) + ", type=" + tokenTypeName(typeOfValue(value));
        errors::UnexpectedBehaviourError(msg);
    }   

    }
}


#define DO_STORE_RESULT true
#define DONT_STORE_RESULT false


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
static const OpCode systemInterrupts[] =
{
    OpCode::PRINT,
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
    // treat token's value as a pointer to an array of token pointers
    Token** operands = (Token**) token->value; 
    OpType opType = operatorType(token->opCode);
    
    // control flow statements parse themselves their own operands
    // free scopes do not have token operands, but syntax trees
    if (!isFlowOp(token->opCode) && token->opCode != OpCodes::PUSH_SCOPE)
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
        // flow operators do not have a result
        || isFlowOp(token->opCode)
        // scopes do not have a result (and are not operations)
        || token->opCode == OpCodes::PUSH_SCOPE
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

    // scope tokens do not have a Token** as value, they have a SyntaxTree* instead
    // it will be deleted in the byteCodeFor() function
    if (token->opCode != OpCodes::PUSH_SCOPE)
    {
        // delete operands since they won't be accessed anymore after their compilation
        delete[] operands;
    }
    
}


static void byteCodeForUnaryOperation(Tokens::Token** operands, OpCode opCode, ByteList& byteList)
{
    using namespace symbol_table;
    
    if (hasReturnValueInRegister(operands[0]))
    {
        switch (operands[0]->type)
        {    
        case TokenType::DOUBLE:
        case TokenType::LONG:
            AddNode(OpCode::REG_MOV_8);
            break;
        
        case TokenType::INT:
        case TokenType::FLOAT:
            AddNode(OpCode::REG_MOV_4);
            break;
        
        case TokenType::BYTE:
            AddNode(OpCode::REG_MOV_1);
            break;
        
        case TokenType::BOOL:
            AddNode(OpCode::REG_MOV_BIT);
            break;
        }
        AddNode(Registers::GENERAL_A);
        AddNode(Registers::RESULT);
    }
    else if (operands[0]->opCode == OpCodes::REFERENCE)
    {
        switch (operands[0]->type)
        {    
        case TokenType::DOUBLE:
        case TokenType::LONG:
            AddNode(OpCode::LD_A_8);
            break;
        
        case TokenType::INT:
        case TokenType::FLOAT:
            AddNode(OpCode::LD_A_4);
            break;
        
        case TokenType::BYTE:
            AddNode(OpCode::LD_A_1);
            break;
        
        case TokenType::BOOL:
            AddNode(OpCode::LD_A_BIT);
            break;
        }
        AddNode(StackPositionOf(operands[0]), 8);
    }
    else // literal
    {
        addConstLoad(Registers::GENERAL_A, operands[0]->value, byteList);
    }

    if (opCode != OpCode::NO_OP)
    {
        AddNode(opCode);
    }

}


// operation size: 19 bytes
static void byteCodeForBinaryOperation(Tokens::Token** operands, OpCode opCode, ByteList& byteList)
{
    using namespace symbol_table;

    if (hasReturnValueInRegister(operands[0]))
    {
        switch (tokenTypeOf(operands[0]))
        {    
        case TokenType::DOUBLE:
        case TokenType::LONG:
            AddNode(OpCode::REG_MOV_8);
            break;
        
        case TokenType::INT:
        case TokenType::FLOAT:
            AddNode(OpCode::REG_MOV_4);
            break;
        
        case TokenType::BYTE:
            AddNode(OpCode::REG_MOV_1);
            break;
        
        case TokenType::BOOL:
            AddNode(OpCode::REG_MOV_BIT);
            break;
        }
        AddNode(Registers::GENERAL_A);
        AddNode(Registers::RESULT);
    }
    else if (operands[0]->opCode == OpCodes::REFERENCE)
    {
        
        switch (tokenTypeOf(operands[0]))
        {    
        case TokenType::DOUBLE:
        case TokenType::LONG:
            AddNode(OpCode::LD_A_8);
            break;
        
        case TokenType::INT:
        case TokenType::FLOAT:
            AddNode(OpCode::LD_A_4);
            break;
        
        case TokenType::BYTE:
            AddNode(OpCode::LD_A_1);
            break;
        
        case TokenType::BOOL:
            AddNode(OpCode::LD_A_BIT);
            break;
        }
        AddNode(StackPositionOf(operands[0]), 8);
    }
    else // literal
    {
        addConstLoad(Registers::GENERAL_A, operands[0]->value, byteList); 
    }

    if (hasReturnValueInRegister(operands[1]))
    {
        switch (tokenTypeOf(operands[1]))
        {    
        case TokenType::DOUBLE:
        case TokenType::LONG:
            AddNode(OpCode::REG_MOV_8);
            break;
        
        case TokenType::INT:
        case TokenType::FLOAT:
            AddNode(OpCode::REG_MOV_4);
            break;
        
        case TokenType::BYTE:
            AddNode(OpCode::REG_MOV_1);
            break;
        
        case TokenType::BOOL:
            AddNode(OpCode::REG_MOV_BIT);
            break;
        }
        AddNode(Registers::GENERAL_B);
        AddNode(Registers::RESULT);
    }
    else if (operands[1]->opCode == OpCodes::REFERENCE)
    {
        
        switch (tokenTypeOf(operands[1]))
        {    
        case TokenType::DOUBLE:
        case TokenType::LONG:
            AddNode(OpCode::LD_B_8);
            break;
        
        case TokenType::INT:
        case TokenType::FLOAT:
            AddNode(OpCode::LD_B_4);
            break;
        
        case TokenType::BYTE:
            AddNode(OpCode::LD_B_1);
            break;
        
        case TokenType::BOOL:
            AddNode(OpCode::LD_B_BIT);
            break;
        }
        AddNode(StackPositionOf(operands[1]), 8);
    }
    else // literal
    {
        addConstLoad(Registers::GENERAL_B, operands[1]->value, byteList);
    }

    if (opCode != OpCode::NO_OP)
    {
        AddNode(opCode);
    }

}


static std::string* storeLiteral(Value value, Tokens::TokenType type, ByteList& byteList)
{
    using namespace symbol_table;
    
    // declare a new Symbol holding the value
    // push the operation result onto the stack

    // TODO string pointer should be deleted later

    std::string* name = getTmpSymbolName();

    SymbolTable::declare(
        name,
        new Symbol(0, type)
    );

    // choose instruction based on the data type
    switch (type)
    {
    case TokenType::LONG:
    case TokenType::DOUBLE:
        AddNode(OpCode::MEM_SET_8);
        break;
    
    case TokenType::INT:
    case TokenType::FLOAT:
        AddNode(OpCode::MEM_SET_4);
        break;
    
    case TokenType::BYTE:
            AddNode(OpCode::MEM_SET_1);
            break;
    
    case TokenType::BOOL:
        AddNode(OpCode::MEM_SET_BIT);
        break;
    }

    AddNode(SymbolTable::get(name)->stackPosition, 8);
    AddNode(value, typeSize(type));

    return name;
}


static std::string* storeResult(Registers reg, Tokens::TokenType type, ByteList& byteList)
{
    using namespace symbol_table;

    // declare a new Symbol holding the value
    // push the operation result onto the stack

    // TODO string pointer should be deleted later

    std::string* name = getTmpSymbolName();

    SymbolTable::declare(
        name,
        new Symbol(0, type)
    );

    // choose instruction based on variable type
    switch (type)
    {    
    case TokenType::DOUBLE:
    case TokenType::LONG:
        AddNode(OpCode::REG_MOV_8);
        break;
    
    case TokenType::INT:
    case TokenType::FLOAT:
        AddNode(OpCode::REG_MOV_4);
        break;
    
    case TokenType::BYTE:
            AddNode(OpCode::REG_MOV_1);
            break;
    
    case TokenType::BOOL:
        AddNode(OpCode::REG_MOV_BIT);
        break;
    }

    AddNode(SymbolTable::get(name)->stackPosition, 8);
    AddNode(reg);

    return name;
}


size_t SyntaxTree::byteCodeFor(Tokens::Token* token, Tokens::Token** operands, bool doStoreResult)
{
    using namespace symbol_table;

    switch(token->opCode)
    {

    case OpCodes::ASSIGNMENT_ASSIGN:
    {
        Symbol* lValue = SymbolTable::get((std::string*) operands[0]->value);

        if (hasReturnValueInRegister(operands[1]))
        {
            switch (tokenTypeOf(operands[1]))
            {    
            case TokenType::DOUBLE:
            case TokenType::LONG:
                AddNode(OpCode::REG_MOV_8);
                break;
            
            case TokenType::INT:
            case TokenType::FLOAT:
                AddNode(OpCode::REG_MOV_4);
                break;
            
            case TokenType::BYTE:
                AddNode(OpCode::REG_MOV_1);
                break;
            
            case TokenType::BOOL:
                AddNode(OpCode::REG_MOV_BIT);
                break;
            }
            AddNode(lValue->stackPosition, 8);
            AddNode(Registers::RESULT);
        }
        else if (operands[1]->opCode == OpCodes::REFERENCE)
        {
            switch (tokenTypeOf(operands[1]))
            {
            case TokenType::LONG:
            case TokenType::DOUBLE:
                AddNode(OpCode::MEM_MOV_8);
                break;
            
            case TokenType::INT:
            case TokenType::FLOAT:
                AddNode(OpCode::MEM_MOV_4);
                break;
            
            case TokenType::BYTE:
                AddNode(OpCode::MEM_MOV_1);
                break;
            
            case TokenType::BOOL:
                AddNode(OpCode::MEM_MOV_BIT);
                break;
            };
            AddNode(lValue->stackPosition, 8);
            AddNode(StackPositionOf(operands[1]), 8);
        }
        else // token is a literal
        {
            // switch TokenType of the variable the value is beign assigned to
            // because literal values have TokenType::NUMERIC type
            // size compatibility checks are performed during the SyntaxTree building phase
            switch (tokenTypeOf(operands[0]))
            {
            case TokenType::LONG:
            case TokenType::DOUBLE:
                AddNode(OpCode::MEM_SET_8);
                break;
            
            case TokenType::INT:
            case TokenType::FLOAT:
                AddNode(OpCode::MEM_SET_4);
                break;
            
            case TokenType::BYTE:
                AddNode(OpCode::MEM_SET_1);
                break;
            
            case TokenType::BOOL:
                AddNode(OpCode::MEM_SET_BIT);
                break;
            }
            AddNode(lValue->stackPosition, 8);
            // pass as second operand the token's literal value
            // the size is that of the variable the value is being assigned to
            // size compatibility checks have already been performed
            AddNode(operands[1]->value, typeSize(operands[0]->type));
        }
        
        // these operands won't be accessed anymore after compilation
        deleteOperands(operands, OpType::BINARY);

        return lValue->stackPosition;
    }


    case OpCodes::ARITHMETICAL_SUM:
    {
        byteCodeForBinaryOperation(operands, OpCode::ADD, byteList);

        TokenType type = tokenTypeOf(operands[0]);

        deleteOperands(operands, OpType::BINARY);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, type, byteList);
        }
        
        setReturnValueToRegister(token);

        // sum operations do not return any stack pointer
        // since the result is stored in the result register
        return 0;
    }


    case OpCodes::ARITHMETICAL_INC:
    {

        Token one = Token(TokenType::INT, 0, OpCodes::LITERAL, 1);
        Token* ops[2] = {operands[0], &one};

        byteCodeForBinaryOperation(ops, OpCode::ADD, byteList);

        TokenType type = tokenTypeOf(operands[0]);

        // update the incremented variable
        switch (type)
        {    
        case TokenType::DOUBLE:
        case TokenType::LONG:
            AddNode(OpCode::REG_MOV_8);
            break;
        
        case TokenType::INT:
        case TokenType::FLOAT:
            AddNode(OpCode::REG_MOV_4);
            break;

        case TokenType::BYTE:
            AddNode(OpCode::REG_MOV_1);
            break;
            
        case TokenType::BOOL:
            AddNode(OpCode::REG_MOV_BIT);
            break;
        }
        AddNode(StackPositionOf(operands[0]), 8);
        AddNode(Registers::RESULT);

        deleteOperands(operands, OpType::UNARY);
        
        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, type, byteList);
        }
        
        setReturnValueToRegister(token);
        
        return 0;
    }


    case OpCodes::ARITHMETICAL_DEC:
    {

        Token one = Token(TokenType::INT, 0, OpCodes::LITERAL, 1);
        Token* ops[2] = {operands[0], &one};

        byteCodeForBinaryOperation(ops, OpCode::SUB, byteList);

        TokenType type = tokenTypeOf(operands[0]);

        // update the decremented variable
        switch (type)
        {    
        case TokenType::DOUBLE:
        case TokenType::LONG:
            AddNode(OpCode::REG_MOV_8);
            break;
        
        case TokenType::INT:
        case TokenType::FLOAT:
            AddNode(OpCode::REG_MOV_4);
            break;
        
        case TokenType::BYTE:
            AddNode(OpCode::REG_MOV_1);
            break;
        
        case TokenType::BOOL:
            AddNode(OpCode::REG_MOV_BIT);
            break;
        }
        AddNode(StackPositionOf(operands[0]), 8);
        AddNode(Registers::RESULT);

        deleteOperands(operands, OpType::UNARY);
        
        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, type, byteList);
        }
        
        setReturnValueToRegister(token);
        
        return 0;
    }


    case OpCodes::ARITHMETICAL_SUB:
    {
        byteCodeForBinaryOperation(operands, OpCode::SUB, byteList);

        TokenType type = tokenTypeOf(operands[0]);

        deleteOperands(operands, OpType::BINARY);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, type, byteList);
        }
        
        setReturnValueToRegister(token);
        
        return 0;
    }


    case OpCodes::ARITHMETICAL_MUL:
    {
        byteCodeForBinaryOperation(operands, OpCode::MUL, byteList);

        TokenType type = tokenTypeOf(operands[0]);

        deleteOperands(operands, OpType::BINARY);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, type, byteList);
        }
        
        setReturnValueToRegister(token);

        return 0;
    }


    case OpCodes::ARITHMETICAL_DIV:
    {
        byteCodeForBinaryOperation(operands, OpCode::DIV, byteList);
    
        TokenType type = tokenTypeOf(operands[0]);

        deleteOperands(operands, OpType::BINARY);

        if (doStoreResult)
        {
            return (size_t) storeResult(Registers::RESULT, type, byteList);
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

        AddNode(OpCode::IF_JUMP);
        // 3 is the size of the REG_TO_REG instruction along with its operands
        AddNode(SymbolTable::getStackPointer() + 3, 8);

        AddNode(OpCode::REG_TO_REG);
        AddNode(Registers::ZERO_FLAG);
        AddNode(Registers::SIGN_FLAG);

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

        AddNode(OpCode::IF_JUMP);
        // 3 is the size of the REG_TO_REG instruction along with its operands
        AddNode(SymbolTable::getStackPointer() + 3, 8);

        AddNode(OpCode::REG_TO_REG);
        AddNode(Registers::ZERO_FLAG);
        AddNode(Registers::SIGN_FLAG);

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

        AddNode(OpCode::IF_JUMP);
        // address to jump to = current stack pointer + size of instructions to jump over
        AddNode(SymbolTable::getStackPointer() + 19, 8);

        Token zero = Token(TokenType::INT, 0, OpCodes::LITERAL, 0);
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

        AddNode(OpCode::IF_JUMP);
        // address to jump to = current stack pointer + size of instructions to jump over
        AddNode(SymbolTable::getStackPointer() + 19, 8);

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

        AddNode(code);

        deleteOperands(operands, OpType::UNARY);

        return 0;
    }


    case OpCodes::SYSTEM_LOAD:
    {
        byteCodeForUnaryOperation(operands, OpCode::NO_OP, byteList);

        deleteOperands(operands, OpType::UNARY);

        return 0;
    }


    case OpCodes::OPEN_PARENTHESIS:
    {
        // save operand's properties to avoid accessing freed memory
        const TokenType contentType = tokenTypeOf(operands[0]);

        if (doStoreResult)
        {
            Value contentValue = operands[0]->value;

            deleteOperands(operands, OpType::UNARY);

            if (hasReturnValueInRegister(operands[0]))
            {
                // copy return value from register to memory
                return (size_t) storeResult(Registers::RESULT, contentType, byteList);
            }

            if (token->opCode == OpCodes::REFERENCE)
            {
                return SymbolTable::get((std::string*) contentValue)->stackPosition;
            }
            
            return (size_t) storeLiteral(contentValue, contentType, byteList);
        }

        // if not storeResult:

        if (operands[0]->opCode == OpCodes::REFERENCE)
        {
            // if parenthesis content is a reference, load it into the correct register and return 0

            switch (contentType)
            {
            case TokenType::BOOL:
                // if bool --> load result into a bit register (ZERO FLAG)
                AddNode(OpCode::LD_ZERO_FLAG);
                break;
            
            case TokenType::BYTE:
                AddNode(OpCode::LD_RESULT_1);
                break;
            
            case TokenType::INT:
            case TokenType::FLOAT:
                AddNode(OpCode::LD_RESULT_4);
                break;
            
            case TokenType::LONG:
            case TokenType::DOUBLE:
                AddNode(OpCode::LD_RESULT_8);
            }

            AddNode(SymbolTable::get(IdOf(operands[0]))->stackPosition, 8);
            
            // to have the content's result readily available for the next operator
            setReturnValueToRegister(token);

            deleteOperands(operands, OpType::UNARY);

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
            - invert boolean condition
            - conditional jump based on condition
            - if body block
        */
    
    // compile boolean condition

        parseTokenOperator(operands[0]);

    // invert the boolean condition

        // load condition
        if (hasReturnValueInRegister(operands[0]))
        {
            AddNode(OpCode::REG_TO_REG);
            AddNode(Registers::GENERAL_A);
            AddNode(Registers::ZERO_FLAG);
        }
        else if (operands[0]->opCode == OpCodes::LITERAL)
        {
            addConstLoad(Registers::GENERAL_A, operands[0]->value, byteList);
        }
        else if (operands[0]->opCode == OpCodes::REFERENCE)
        {
            switch (tokenTypeOf(operands[0]))
            {    
            case TokenType::DOUBLE:
            case TokenType::LONG:
                AddNode(OpCode::LD_A_8);
                break;
            
            case TokenType::INT:
            case TokenType::FLOAT:
                AddNode(OpCode::LD_A_4);
                break;      
            
            case TokenType::BYTE:
                AddNode(OpCode::LD_A_1);
                break;
            
            case TokenType::BOOL:
                AddNode(OpCode::LD_A_BIT);
                break;
            }

            AddNode(StackPositionOf(operands[0]), 8);
        }

        // invert condition (compare with 0)
        addConstLoad(Registers::GENERAL_B, 0, byteList);
        AddNode(OpCode::CMP);      
        
    // conditional jump instruction

        AddNode(OpCode::IF_JUMP);
        // bodySizeNode will be set later when the actual if body is compiled
        // 8 is the size in bytes of size_t, 0 is just to initialize it
        ByteNode* exitIndexNode = new ByteNode(0, 8);
        byteList.add(exitIndexNode);

        // if the if's body is a whole scope, extraxt its SyntaxTree and extend
        // this' byteList with the scope tree's
        if (operands[1]->opCode == OpCodes::PUSH_SCOPE)
        {
            SyntaxTree* body = (SyntaxTree*) operands[1]->value;
            // body has already been parsed to byte code
            byteList.extend(body->byteList);

            // body's SyntaxTree won't be used anymore
            delete body;
        }
        else // if operand is just a statement without scope
        {
            parseTokenOperator(operands[1]);

            delete operands[1];
        }

        // update the index to jump to
        exitIndexNode->data = byteList.getCurrentSize();

        // delete just the first operand (bool condition) since the if's body has already
        // been deleted after being handled
        delete operands[0];

        return 0;
    }


    case OpCodes::FLOW_WHILE:
    {
        /*
            - evaluate condition
            - code for body
            - jump to condition
        
        @l1:
            // boolean condition
            if not condition jump @l2
            // while body
            jump @l1
        @l2:
        */

    // compile boolean operation

        // save the boolean condition's instruction index
        const size_t conditionInstructionIndex = byteList.getCurrentSize();

        parseTokenOperator(operands[0]);


    // invert the boolean condition

        // load condition
        if (hasReturnValueInRegister(operands[0]))
        {
            AddNode(OpCode::REG_TO_REG);
            AddNode(Registers::GENERAL_A);
            AddNode(Registers::ZERO_FLAG);
        }
        else if (operands[0]->opCode == OpCodes::LITERAL)
        {
            addConstLoad(Registers::GENERAL_A, operands[0]->value, byteList);
        }
        else if (operands[0]->opCode == OpCodes::REFERENCE)
        {
            switch (tokenTypeOf(operands[0]))
            {    
            case TokenType::DOUBLE:
            case TokenType::LONG:
                AddNode(OpCode::LD_A_8);
                break;
            
            case TokenType::INT:
            case TokenType::FLOAT:
                AddNode(OpCode::LD_A_4);
                break;
            
            case TokenType::BYTE:
                AddNode(OpCode::LD_A_1);
                break;
            
            case TokenType::BOOL:
                AddNode(OpCode::LD_A_BIT);
                break;
            }

            AddNode(StackPositionOf(operands[0]), 8);
        }

        // invert condition (compare with 0)
        addConstLoad(Registers::GENERAL_B, 0, byteList);
        AddNode(OpCode::CMP);        
    
    // conditional jump

        AddNode(OpCode::IF_JUMP);
        // save condidional jump's operand's pointer to set it later
        ByteNode* exitIndexNode = new ByteNode(0, 8);
        byteList.add(exitIndexNode);

    // while body

        if (operands[1]->opCode == OpCodes::PUSH_SCOPE)
        {
            SyntaxTree* body = (SyntaxTree*) operands[1]->value;
            byteList.extend(body->byteList);
            delete body;
        }
        else
        {
            parseTokenOperator(operands[1]);
            delete operands[1];
        }

        // add unconditional jump to condition evaluation
        AddNode(OpCode::JMP);
        AddNode(conditionInstructionIndex, 8);

        // set exit index to the byte next to the unconditional jump instruction
        exitIndexNode->data = byteList.getCurrentSize();

        delete operands[0];

        return 0;
    }


    case OpCodes::PUSH_SCOPE:
    {
        SyntaxTree* tree = (SyntaxTree*) token->value;
        byteList.extend(tree->byteList);

        // after the tree's byte code is extracted, the tree won't be used anymore
        delete tree;

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

