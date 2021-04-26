#include "tac.hh"
#include "utils.hh"
#include "op_codes.hh"
#include "token.hh"
#include "syntax_tree.hh"


// converts a token->value (real reference) to a const Address*
#define toAddress(reference) ((const Address*) reference)

#define isReference(token) ((TacValueType) (token->opCode == OpCodes::REFERENCE))


using namespace tac;


TacInstruction* Tac::parseTree(syntax_tree::SyntaxTree& tree, bool addLabel)
{
    using namespace syntax_tree;
    using namespace Tokens;

    TacInstruction* label = nullptr;

    if (addLabel)
    {
        // the first instruction for the SyntaxTree is a label 
        // pointing to the start of the instructions for said SyntaxTree
        label = new TacInstruction(TacOp::LABEL);
        // add the label to the three address code
        add(label);
    }

    for (Statement* statement = tree.statements.start; statement != nullptr; statement = statement->next)
    {   
        // iterate over operators in the statement
        for (Token* root = statement->root; root != nullptr; root = root->next)
        {
            if (isOperator(root->opCode))
            {
                parseOperator(root);
            }
        }
    }

    /*
        return label pointing to the first instruction for the parsed SyntaxTree
        or nullptr if parameter addLabel is false
    */
    return label;
    
}


TacInstruction* Tac::parseOperator(Tokens::Token* token)
{
    using namespace Tokens;

    /*
        check if token is a scope and handle it differently 
        from regular tokens
        this scope is independent from any operator, since
        operators that require scopes should parse the scopes they
        require in the tacFor() function when and how they need it
    */
    if (token->opCode == OpCodes::PUSH_SCOPE)
    {
        using namespace syntax_tree;

        // Scope Token's value is a SyntaxTree* of its content
        SyntaxTree* tree = (SyntaxTree*) token->value;

        return parseTree(*tree, NO_LABEL);
    }


    // treat token's value as a pointer to an array of token pointers
    Token** operands = (Token**) token->value; 
    
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
            parseOperator(operand);
        }

    }

    // save last TacInstruction to be returned later
    TacInstruction* first = instructions;

    // generate three address code for the operator
    token->value = toValue(tacFor(token, operands));
    token->opCode = OpCodes::REFERENCE;

    // return the first TacInstruction generated for the operator Token
    return first->next;
}


const Address* Tac::tacFor(OpCodes opCode, Tokens::Token** operands)
{
    using namespace Tokens;

    // instantiate a temporary token to feed into the function
    Token token = Token(TokenType::NONE, 0, opCode);
    // return the result of the other tacFor() which takes the token as argument
    return tacFor(&token, operands);
}


const Address* Tac::tacFor(Tokens::Token* token, Tokens::Token** operands)
{
    using namespace Tokens;

    OpCodes opCode = token->opCode;

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


        case OpCodes::PARENTHESIS:
        {
            /*
                // just the content of parenthesis
                r = a
            */

            const Address* result = Address::getAddress();

            add(new TacInstruction(
                TacOp::ASSIGN,
                new TacValue(TacValueType::ADDRESS, toValue(result)),
                new TacValue(isReference(operands[0]), operands[0]->value)
            ));

            return result;
        }

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
                if r jump @l1:  
                jump @l2         // first is false --> whole statement is false
            @l1:
                r = b == 1
            @l2:

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
                if r jump @l1
                r = b == 1
            @l1:

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
                // just flip the operands

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

        case OpCodes::FLOW_IF:
        {
            // if-else statement
            /*
                a = !a
                if a jump @l1
                ...             // TAC for if body
                jump @l2
            @l1:
                ...             // TAC for else body
            @l2:

            */
            // if statement
            /*
                a = !a          // invert condition
                if a jump @l1   // @l1 is label to the end of the scope     
                ...             // TAC for if body
            @l1:
                
            */

            using namespace syntax_tree;

            // create a label to jump to if condition is false
            TacInstruction* l1 = new TacInstruction(TacOp::LABEL);

            // operands for LOGICAL_NOT to invert boolean condition
            Token* ops[1] = { operands[0] };

            add(new TacInstruction(
                TacOp::IF,
                new TacValue(TacValueType::ADDRESS,
                    toValue(tacFor(OpCodes::LOGICAL_NOT, ops))),
                new TacValue(TacValueType::LABEL, toValue(l1))
            ));

            // retrieve scope operand (which is always a pointer --> 8 bytes long in x86_64 machines)
            SyntaxTree* scopeTree = (SyntaxTree*) operands[1]->value;

            // parse the scope SyntaxTree and do not generate any label
            parseTree(*scopeTree, NO_LABEL);

            // delete the SyntaxTree since it won't be used anymore
            delete scopeTree;

            // differenciate between if and if-else statement
            Token* elseToken = token->next;
            if (elseToken != nullptr && elseToken->opCode == OpCodes::FLOW_ELSE)       
            {
                // label for after the else body
                TacInstruction* l2 = new TacInstruction(TacOp::LABEL);

                // jump @l2
                add(new TacInstruction(
                    TacOp::JUMP,
                    new TacValue(TacValueType::LABEL, toValue(l2))
                ));

                // label for else body
                add(l1);

                // TAC for else body

                // treat elseToken's value as an array of Token*
                Token** elseOperands = (Token**) elseToken->value;
                // treat the first operand of elseToekn as a SyntaxTree* (since it is)
                SyntaxTree* elseBody = (SyntaxTree*) elseOperands[0]->value;
                // parse the body of the else statement (adding the resulting TAC instructions)
                parseTree(*elseBody, NO_LABEL);
                
                // delete the SyntaxTree since it won't be used anymore
                delete elseBody;

                // remove else token from Statement (Token doubly-linked list)
                Token::removeToken(elseToken);
                
                // delete else since it won't be evaluated on its own
                delete elseToken;

                // label for exiting the statement
                add(l2);

                // if statements shouldn't return anything
                return nullptr;
            }

            // whereas if statement is just a bare if
       
            // finally add the label instruction to jump to if condition is false
            add(l1);
            
            // if statements shouldn't return anything
            return nullptr;
        }

    } // switch (token->opCode)

}