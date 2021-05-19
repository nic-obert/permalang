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
        else if (operands[1]->opCode == OpCodes::LITERAL)
        {
            byteList.add(new ByteNode(OpCode::MEM_SET));
            byteList.add(new ByteNode(lValue->stackPosition));
            // pass as second operand the token's literal value
            byteList.add(new ByteNode(operands[1]->value));
        }
        
        return lValue->stackPosition;
    }

    } // switch(token->opCode)


}

