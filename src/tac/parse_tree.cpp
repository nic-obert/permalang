#include "pch.hh"

#include "tac.hh"


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

    // generate three address code for the operator and
    // store it as the token's value
    token->value = toValue(tacFor(token, operands));
    // token is now a reference to its operation's result
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
                jump @Lexit         // first is false --> whole statement is false
            @l1:
                r = b == 1
            @Lexit:

            */
                
            // create array of operands
            Token op2 = Token(TokenType::INT, 0, OpCodes::LITERAL, 1);
            Token* ops[2] = { operands[0], &op2 };

            TacInstruction* l1 = new TacInstruction(TacOp::LABEL);
            TacInstruction* exitLabel = new TacInstruction(TacOp::LABEL);

            const Address* result = tacFor(OpCodes::LOGICAL_EQ, ops);

            add(new TacInstruction(
                TacOp::IF,
                new TacValue(TacValueType::ADDRESS, toValue(result)),
                new TacValue(TacValueType::LABEL, toValue(l1))
            ));
            add(new TacInstruction(
                TacOp::JUMP,
                new TacValue(TacValueType::LABEL, toValue(exitLabel))
            ));

            add(l1);

            ops[0] = operands[0];
            
            result = tacFor(OpCodes::LOGICAL_EQ, ops);

            add(exitLabel);

            return result;
        }

        case OpCodes::LOGICAL_OR:
        {
            /*
                r = a == 1
                if r jump @Lexit
                r = b == 1
            @Lexit:

            */

            TacInstruction* exitLabel = new TacInstruction(TacOp::LABEL);

            Token op2 = Token(TokenType::INT, 0, OpCodes::LITERAL, 1);
            Token* ops[2] = { operands[0], &op2 };

            const Address* result = tacFor(OpCodes::LOGICAL_EQ, ops);

            add(new TacInstruction(
                TacOp::IF,
                new TacValue(TacValueType::ADDRESS, toValue(result)),
                new TacValue(TacValueType::LABEL, toValue(exitLabel))
            ));

            ops[0] = operands[1];

            result = tacFor(OpCodes::LOGICAL_EQ, ops);

            add(exitLabel);

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
                if r jump @Lexit
                r = a == b
            @Lexit:

            */

            TacInstruction* exitLabel = new TacInstruction(TacOp::LABEL);

            const Address* result = tacFor(OpCodes::LOGICAL_LESS, operands);

            add(new TacInstruction(
                TacOp::IF,
                new TacValue(TacValueType::ADDRESS, toValue(result)),
                new TacValue(TacValueType::LABEL, toValue(exitLabel))
            ));

            result = tacFor(OpCodes::LOGICAL_EQ, operands);

            add(exitLabel);

            return result;
        }

        case OpCodes::LOGICAL_GREATER_EQ:
        {
            /*
                r = a > b
                if r jump @Lexit
                r = a == b
            @Lexit:

            */

            TacInstruction* exitLabel = new TacInstruction(TacOp::LABEL);

            const Address* result = tacFor(OpCodes::LOGICAL_GREATER, operands);

            add(new TacInstruction(
                TacOp::IF,
                new TacValue(TacValueType::ADDRESS, toValue(result)),
                new TacValue(TacValueType::LABEL, toValue(exitLabel))
            ));

            result = tacFor(OpCodes::LOGICAL_EQ, operands);

            add(exitLabel);

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
            /*
                - check for any following if statement
                - add if statements along with their condition
                  to a queue of statements to be evaluated
                - generate TAC for the just gathered statements
                - finally add the trailing else statement, if present
            */

            /*
                c1 = !c1            // c1 is the condition of first if
                if c1 jump @if2
                ...                 // TAC for first if body
                jump @Lexit

            @if2:
                c2 = !c2
                if c2 jump @if3
                ...                 // TAC for second if body       
                jump @Lexit
            
            @if3:
                ...                 // and so on...

            @Lelse:
                ...

            @Lexit:

            */
            

            using namespace syntax_tree;

            // FIFO queue of if statements
            std::queue<std::pair<Token*, SyntaxTree*>> ifQueue = 
                std::queue<std::pair<Token*, SyntaxTree*>>();
            
            // declare in outer scope since this will be the trailing "else" token, if present
            Token* tok = token;
            while (true)
            {
                if (tok->opCode == OpCodes::FLOW_ELSE)
                {   
                    // check for an "if" following the "else"
                    if (tok->next->opCode == OpCodes::FLOW_IF)
                    {
                        tok = tok->next;
                        continue;
                    }

                    // if there is no further "if", break the loop
                    // tok is now a pointer to the trailing else statement token
                    break;
                }

                // only add if statements
                if (tok->opCode != OpCodes::FLOW_IF)
                {
                    break;
                }
            
                // if this code is reached tok is always an "if"

                // take operands
                Token** ops = (Token**) tok->value;
                // enqueue pair of condition-body
                ifQueue.emplace(std::pair<Token*, SyntaxTree*>(ops[0], (SyntaxTree*) ops[1]->value));
                
                // go to next token
                tok = tok->next;
                
            } // loop for enqueueing the condidion-body pairs


            // generate exit label to be added later
            TacInstruction* exitLabel = new TacInstruction(TacOp::LABEL);
            // "@if*" labels for reference
            TacInstruction* lastLabel;

            while (!ifQueue.empty())
            {
                // create a new label for the next statement
                lastLabel = new TacInstruction(TacOp::LABEL);

                // retrieve the condition-body pair from the queue
                std::pair<Token*, SyntaxTree*> statement = ifQueue.front();
                // pop it out
                ifQueue.pop();
                
                Token** ops = (Token**) statement.first->value;

                // add TAC for condition if it's an operator itself
                if (isOperator(ops[0]->opCode))
                {
                    parseOperator(ops[0]);
                }

                // differenciate between reference and literal for optimization purposes
                const Address* condition;
                if (ops[0]->opCode == OpCodes::REFERENCE)
                {
                    condition = toAddress(ops[0]->value);

                    // invert the statement's condition
                    add(new TacInstruction(
                        TacOp::EQ,
                        new TacValue(TacValueType::ADDRESS, ops[0]->value),
                        new TacValue(TacValueType::ADDRESS, ops[0]->value),
                        new TacValue(TacValueType::LITERAL, 0)
                    ));                    
                }
                else // if (ops[0]->opCode != OpCodes::REFERENCE)
                {
                    condition = Address::getAddress();

                    // invert the statement's condition
                    add(new TacInstruction(
                        TacOp::EQ,
                        new TacValue(TacValueType::ADDRESS, toValue(condition)),
                        new TacValue(TacValueType::LITERAL, ops[0]->value),
                        new TacValue(TacValueType::LITERAL, 0)
                    ));
                }

                // add conditional jump to the next if
                add(new TacInstruction(
                    TacOp::IF,
                    new TacValue(TacValueType::ADDRESS, toValue(condition)),
                    new TacValue(TacValueType::LABEL, toValue(lastLabel))
                ));

                // add the statement's body
                parseTree(*statement.second, NO_LABEL);

                // add an unconditional jump to the exit label
                add(new TacInstruction(
                    TacOp::JUMP,
                    new TacValue(TacValueType::LABEL, toValue(exitLabel))
                ));
                
                // add the next statement's label
                add(lastLabel);

            } // while (!ifQueue.empty())

            // check if a trailing "else" is present
            if (tok->opCode == OpCodes::FLOW_ELSE)
            {
                // label for else is already in place
                // extract the SyntaxTree* from else token
                SyntaxTree* elseBody = (SyntaxTree*) ((Token**) tok->value)[0]->value;

                // add else body
                parseTree(*elseBody, NO_LABEL);

            }

            // finally add the exit label
            add(exitLabel);
            

            // if statements do not have a return value
            return nullptr;
        }   

    } // switch (token->opCode)

}