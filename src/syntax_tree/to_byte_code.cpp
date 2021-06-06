#include "syntax_tree.hh"
#include "pvm.hh"
#include "errors.hh"


#define hasReturnValueInRegister(token) (token->priority == 1)

#define setReturnValueToRegister(token) token->priority = 1

// extracts the std::string* identifier from a Token
#define IdOf(token) ((std::string*) token->value)

#define AddNode(...) byteList.add(new ByteNode(__VA_ARGS__))

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
        // store result only for unary increment and decrement operations
        // don't store result for other unary operations
        (opType == OpType::UNARY && (token->opCode != OpCodes::ARITHMETICAL_INC && token->opCode != OpCodes::ARITHMETICAL_DEC))
        || opType == OpType::STANDALONE
        || isAssignmentOp(token->opCode)
        // flow operators do not have a result
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
        AddNode(OpCode::REG_MOV);
        AddNode(Registers::GENERAL_A);
        AddNode(Registers::RESULT);
    }
    else if (operands[0]->opCode == OpCodes::REFERENCE)
    {
        
        AddNode(OpCode::LD_A);
        AddNode(SymbolTable::get(IdOf(operands[0]))->stackPosition);
    }
    else
    {
        AddNode(OpCode::LD_CONST_A);
        AddNode(operands[0]->value); 
    }

    if (opCode != OpCode::NO_OP)
    {
        AddNode(opCode);
    }

}


// operation size: 19 bytes
static void byteCodeForBinaryOperation(Tokens::Token** operands, pvm::OpCode opCode, pvm::ByteList& byteList)
{
    using namespace pvm;
    using namespace symbol_table;

    if (hasReturnValueInRegister(operands[0]))
    {
        AddNode(OpCode::REG_MOV);
        AddNode(Registers::GENERAL_A);
        AddNode(Registers::RESULT);
    }
    else if (operands[0]->opCode == OpCodes::REFERENCE)
    {
        
        AddNode(OpCode::LD_A);
        AddNode(SymbolTable::get(IdOf(operands[0]))->stackPosition);
    }
    else
    {
        AddNode(OpCode::LD_CONST_A);
        AddNode(operands[0]->value); 
    }

    if (hasReturnValueInRegister(operands[1]))
    {
        AddNode(OpCode::REG_MOV);
        AddNode(Registers::GENERAL_B);
        AddNode(Registers::RESULT);
    }
    else if (operands[1]->opCode == OpCodes::REFERENCE)
    {
        
        AddNode(OpCode::LD_B);
        AddNode(SymbolTable::get(IdOf(operands[1]))->stackPosition);
    }
    else
    {
        AddNode(OpCode::LD_CONST_B);
        AddNode(operands[1]->value); 
    }

    if (opCode != OpCode::NO_OP)
    {
        AddNode(opCode);
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

    AddNode(OpCode::MEM_SET);
    AddNode(SymbolTable::get(name)->stackPosition);
    AddNode(value);

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

    AddNode(OpCode::REG_MOV);
    AddNode(SymbolTable::get(name)->stackPosition);
    AddNode(reg);

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
            AddNode(OpCode::REG_MOV);
            AddNode(lValue->stackPosition);
            AddNode(Registers::RESULT);
        }
        else if (operands[1]->opCode == OpCodes::REFERENCE)
        {
            AddNode(OpCode::MEM_MOV);
            AddNode(lValue->stackPosition);
            AddNode(SymbolTable::get(IdOf(operands[1]))->stackPosition);
        }
        else // token is a literal
        {
            AddNode(OpCode::MEM_SET);
            AddNode(lValue->stackPosition);
            // pass as second operand the token's literal value
            AddNode(operands[1]->value);
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

        Token one = Token(TokenType::INT, 0, OpCodes::LITERAL, 1);
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

        Token one = Token(TokenType::INT, 0, OpCodes::LITERAL, 1);
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

        AddNode(OpCode::IF_JUMP);
        // 3 is the size of the REG_TO_REG instruction along with its operands
        AddNode(SymbolTable::getStackPointer() + 3);

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
        AddNode(SymbolTable::getStackPointer() + 3);

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
        AddNode(SymbolTable::getStackPointer() + 19);

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
        AddNode(SymbolTable::getStackPointer() + 19);

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


    case OpCodes::PARENTHESIS:
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

            if (contentType == TokenType::BOOL)
            {
                // if bool --> load result into a bit register (ZERO FLAG)
                AddNode(OpCode::LD_ZERO_FLAG);
                AddNode(SymbolTable::get(IdOf(operands[0]))->stackPosition);
            }
            else
            {   
                // treat token as a long
                AddNode(OpCode::LD_RESULT);
                AddNode(SymbolTable::get(IdOf(operands[0]))->stackPosition);
            }

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
            - conditional jump based on condition
            - if body block
        */
    
    // compile boolean condition

        parseTokenOperator(operands[0]);

    // invert boolean condition

        // load the boolean condition
        if (hasReturnValueInRegister(operands[0]))
        {
            AddNode(OpCode::REG_TO_REG);
            AddNode(Registers::GENERAL_A);
            AddNode(Registers::ZERO_FLAG);
        }
        else if (operands[0]->opCode == OpCodes::LITERAL)
        {
            AddNode(OpCode::LD_CONST_A);
            AddNode(operands[0]->value);
        }
        else if (operands[0]->opCode == OpCodes::REFERENCE)
        {
            AddNode(OpCode::LD_A);
            AddNode(SymbolTable::get(IdOf(operands[0]))->stackPosition);
        }

        // load 0 to compare the condition with (to invert condition)
        AddNode(OpCode::LD_CONST_B);
        AddNode(0);

        AddNode(OpCode::CMP);
        
    // conditional jump instruction

        AddNode(OpCode::IF_JUMP);
        // bodySizeNode will be set later when the actual if body is compiled
        ByteNode* exitIndexNode = new ByteNode(0);
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
            - invert condition
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

    // invert boolean condition

        // load the boolean condition
        if (hasReturnValueInRegister(operands[0]))
        {
            AddNode(OpCode::REG_TO_REG);
            AddNode(Registers::GENERAL_A);
            AddNode(Registers::ZERO_FLAG);
        }
        else if (operands[0]->opCode == OpCodes::LITERAL)
        {
            AddNode(OpCode::LD_CONST_A);
            AddNode(operands[0]->value);
        }
        else if (operands[0]->opCode == OpCodes::REFERENCE)
        {
            AddNode(OpCode::LD_A);
            AddNode(SymbolTable::get(IdOf(operands[0]))->stackPosition);
        }
        
        // load 0 to compare the condition with (to invert condition)
        AddNode(OpCode::LD_CONST_B);
        AddNode(0);

        AddNode(OpCode::CMP);
    
    // conditional jump

        AddNode(OpCode::IF_JUMP);
        // save condidional jump's operand's pointer to set it later
        ByteNode* exitIndexNode = new ByteNode(0);
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
        AddNode(conditionInstructionIndex);

        // set exit index to the byte next to the unconditional jump instruction
        exitIndexNode->data = byteList.getCurrentSize();

        delete operands[0];

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

