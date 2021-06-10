#include "syntax_tree.hh"
#include "errors.hh"
#include "symbol_table.hh"
#include "keywords.hh"


// for unary operators
typedef enum Side
{
    RIGHT = 0,
    LEFT  = 1
    
} Side;


static const char* const sides[2] = 
{
    "right",
    "left"
};


#define DELETE true

using namespace Tokens;
using namespace syntax_tree;
using namespace symbol_table;


static inline void assertToken(Token* caller, Token* got, TokenType required, Side side)
{
    if (got == nullptr)
    {
        errors::ExpectedTokenError(*caller, required, sides[side]);
    }
    
    if (!isCompatible(tokenTypeOf(got), required))
    {
        errors::TypeError(*caller, required, *got, sides[side]);
    }
}


static inline void assertToken(Token* caller, Token* got, OpCodes required, Side side)
{
    if (got == nullptr)
    {
        errors::ExpectedTokenError(*caller, required, sides[side]);
    }
    
    if (got->opCode != required)
    {
        errors::TypeError(*caller, required, *got, sides[side]);
    }
}


static void binarySatisfy(Token* token, TokenType leftType, TokenType rightType, Statement* statement)
{

    if (token->prev == nullptr)
    {
        errors::ExpectedTokenError(*token, leftType, sides[LEFT]);
    }    
    else if (token->next == nullptr)
    {
        errors::ExpectedTokenError(*token, rightType, sides[RIGHT]);
    }

    if (!isCompatible(tokenTypeOf(token->prev), leftType))
    {
       errors::TypeError(*token, leftType, *token->prev, sides[LEFT]);
    }
    else if (!isCompatible(tokenTypeOf(token->next), rightType))
    {
        errors::TypeError(*token, rightType, *token->next, sides[RIGHT]);
    }

    // pointer to array of token pointers
    token->value = toValue((new Token*[2] {token->prev, token->next}));

    token->type = tokenTypeOf(token->prev);

    // remove tokens to the left and right
    statement->remove(token->prev);
    statement->remove(token->next);

}


static void unarySatisfy(Token* token, TokenType type, Side side, Statement* statement)
{

    if (side == LEFT)
    {
        if (token->prev == nullptr)
        {
            errors::ExpectedTokenError(*token, type, sides[LEFT]);
        }
        if (!isCompatible(tokenTypeOf(token->prev), type))
        {
            errors::TypeError(*token, type, *token->prev, sides[LEFT]);
        }

        token->value = toValue(new Token*[1] {token->prev});
        token->type = tokenTypeOf(token->prev);

        statement->remove(token->prev);

    } 
    else 
    {
        if (token->next == nullptr)
        {
            errors::ExpectedTokenError(*token, type, sides[RIGHT]);
        }
        if (!isCompatible(tokenTypeOf(token->next), type))
        {
            errors::TypeError(*token, type, *token->next, sides[RIGHT]);
        }

        token->value = toValue(new Token*[1] {token->next});
        token->type = tokenTypeOf(token->next);

        statement->remove(token->next);

    }

}



static void declarationSatisfy(Token* token, TokenType type, Statement* statement)
{

    if (token->next == nullptr)
    {
        errors::ExpectedTokenError(*token, type, sides[RIGHT]);
    }
    if (token->next->opCode != OpCodes::REFERENCE)
    {
        errors::TypeError(*token, type, *token->next, sides[RIGHT]);
    }

    // inherit value form next Token (variable's name)
    token->value = token->next->value;
    // type is the TokenType that has been declared
    token->type = type;

    // set next Token's value to nullptr no the std::string value
    // does not get deleted when the Token does
    token->next->value = toValue(nullptr);

    // remove and delete the next Token since it won't be used anymore
    statement->remove(token->next, DELETE);

    token->opCode = OpCodes::REFERENCE;

    SymbolTable::declare(
        (std::string*) token->value,
        new Symbol(0, token->type)
    );

}


static void assignSatisfy(Token* token, Statement* statement)
{

    assertToken(token, token->prev, OpCodes::REFERENCE, LEFT);
    assertToken(token, token->next, tokenTypeOf(token->prev), RIGHT);

    // set token's value to an array of its operands
    token->value = toValue((new Token*[2] {token->prev, token->next}));

    TokenType type = tokenTypeOf(token->prev);

    // update symbol table
    SymbolTable::assign(
        (std::string*) token->prev->value,
        new Symbol(token->next->value, type)
    );

    // type of assignment operator is the type of the variable it has assigned a value to
    token->type = type;

    // remove tokens to the left and right
    statement->remove(token->prev);
    statement->remove(token->next);

}


// increment ++, decrement -- operators
static void incDecSatisfy(Token* token, Statement* statement)
{
    assertToken(token, token->prev, OpCodes::REFERENCE, LEFT);

    token->value = toValue(new Token*[1] { token->prev });
    token->type = tokenTypeOf(token->prev);

    statement->remove(token->prev);

}


static void addressOfSatisfy(Token* token, Statement* statement)
{
    assertToken(token, token->next, OpCodes::REFERENCE, RIGHT);
    
    token->value = toValue(new Token*[1] { token->next });
    token->type = tokenTypeOf(token->next);

    statement->remove(token->next);
}



void SyntaxTree::satisfyToken(Statement* statement, Token* token)
{
    // set token's priority to 0 right away so that it doesn't get evaluated multiple times
    token->priority = 0;

    // satisfy operator's requirements based on it's operation code
    switch (token->opCode)
    {
    case OpCodes::ARITHMETICAL_SUM:
    {
        binarySatisfy(token, TokenType::NUMERIC, TokenType::NUMERIC, statement);

        if (globals::doOptimize)
        {
            Token* op1 = ((Token**) (token->value))[0];
            Token* op2 = ((Token**) (token->value))[1];

            if (op1->opCode == OpCodes::LITERAL && op2->opCode == OpCodes::LITERAL)
            {
                delete[] (Token**) token->value;
                token->value = op1->value + op2->value;
                token->opCode = OpCodes::LITERAL;
                delete op1;
                delete op2;
            }
        }

        break;
    }

    case OpCodes::ARITHMETICAL_SUB:
    {
        binarySatisfy(token, TokenType::NUMERIC, TokenType::NUMERIC, statement);

        if (globals::doOptimize)
        {
            Token* op1 = ((Token**) (token->value))[0];
            Token* op2 = ((Token**) (token->value))[1];

            if (op1->opCode == OpCodes::LITERAL && op2->opCode == OpCodes::LITERAL)
            {
                delete[] (Token**) token->value;
                token->value = op1->value - op2->value;
                token->opCode = OpCodes::LITERAL;
                delete op1;
                delete op2;
            }
        }

        break;
    }

    case OpCodes::ARITHMETICAL_MUL:
    {
        binarySatisfy(token, TokenType::NUMERIC, TokenType::NUMERIC, statement);

        if (globals::doOptimize)
        {
            Token* op1 = ((Token**) (token->value))[0];
            Token* op2 = ((Token**) (token->value))[1];

            // multiplication by 0 is always 0
            if ((op1->opCode == OpCodes::LITERAL && op1->value == 0)
                || (op2->opCode == OpCodes::LITERAL && op2->value == 0))
            {
                delete[] (Token**) token->value;
                token->value = 0;
                token->opCode = OpCodes::LITERAL;
                delete op1;
                delete op2;
            }

            if (op1->opCode == OpCodes::LITERAL && op2->opCode == OpCodes::LITERAL)
            {
                delete[] (Token**) token->value;
                token->value = op1->value * op2->value;
                token->opCode = OpCodes::LITERAL;
                delete op1;
                delete op2;
            }
        }

        break;
    }

    case OpCodes::ARITHMETICAL_DIV:
    {
        binarySatisfy(token, TokenType::NUMERIC, TokenType::NUMERIC, statement);

        if (globals::doOptimize)
        {
            Token* op1 = ((Token**) (token->value))[0];
            Token* op2 = ((Token**) (token->value))[1];

            if (op2->value == 0)
            {
                errors::ZeroDivisionError(*token, *op1, *op2);
            }

            if (op1->opCode == OpCodes::LITERAL && op2->opCode == OpCodes::LITERAL)
            {
                delete[] (Token**) token->value;
                token->value = op1->value / op2->value;
                token->opCode = OpCodes::LITERAL;
                delete op1;
                delete op2;
            }
        }

        break;
    }

    case OpCodes::ARITHMETICAL_POW:
    {
        binarySatisfy(token, TokenType::NUMERIC, TokenType::NUMERIC, statement);
        break;
    }

    case OpCodes::ARITHMETICAL_INC:
    case OpCodes::ARITHMETICAL_DEC:
    {
        incDecSatisfy(token, statement);
        break;
    }


    case OpCodes::LOGICAL_EQ:
    {
        binarySatisfy(token, TokenType::NUMERIC, TokenType::NUMERIC, statement);

        if (globals::doOptimize)
        {
            Token* op1 = ((Token**) (token->value))[0];
            Token* op2 = ((Token**) (token->value))[1];

            if (op1->opCode == OpCodes::LITERAL && op2->opCode == OpCodes::LITERAL)
            {
                delete[] (Token**) token->value;
                token->value = op1->value == op2->value;
                token->opCode = OpCodes::LITERAL;
                delete op1;
                delete op2;
            }
        }

        break;
    }

    case OpCodes::LOGICAL_NOT_EQ:
    {
        binarySatisfy(token, TokenType::NUMERIC, TokenType::NUMERIC, statement);

        if (globals::doOptimize)
        {
            Token* op1 = ((Token**) (token->value))[0];
            Token* op2 = ((Token**) (token->value))[1];

            if (op1->opCode == OpCodes::LITERAL && op2->opCode == OpCodes::LITERAL)
            {
                delete[] (Token**) token->value;
                token->value = op1->value != op2->value;
                token->opCode = OpCodes::LITERAL;
                delete op1;
                delete op2;
            }
        }

        break;
    }

    case OpCodes::LOGICAL_AND:
    {
        binarySatisfy(token, TokenType::BOOL, TokenType::BOOL, statement);

        if (globals::doOptimize)
        {
            Token* op1 = ((Token**) (token->value))[0];
            Token* op2 = ((Token**) (token->value))[1];

            if (op1->opCode == OpCodes::LITERAL && op2->opCode == OpCodes::LITERAL)
            {
                delete[] (Token**) token->value;
                token->value = op1->value && op2->value;
                token->opCode = OpCodes::LITERAL;
                delete op1;
                delete op2;
            }
        }

        break;
    }

    case OpCodes::LOGICAL_OR:
    {
        binarySatisfy(token, TokenType::BOOL, TokenType::BOOL, statement);

        if (globals::doOptimize)
        {
            Token* op1 = ((Token**) (token->value))[0];
            Token* op2 = ((Token**) (token->value))[1];

            if (op1->opCode == OpCodes::LITERAL && op2->opCode == OpCodes::LITERAL)
            {
                delete[] (Token**) token->value;
                token->value = op1->value || op2->value;
                token->opCode = OpCodes::LITERAL;
                delete op1;
                delete op2;
            }
        }

        break;
    }

    case OpCodes::LOGICAL_LESS:
    {
        binarySatisfy(token, TokenType::NUMERIC, TokenType::NUMERIC, statement);

        if (globals::doOptimize)
        {
            Token* op1 = ((Token**) (token->value))[0];
            Token* op2 = ((Token**) (token->value))[1];

            if (op1->opCode == OpCodes::LITERAL && op2->opCode == OpCodes::LITERAL)
            {
                delete[] (Token**) token->value;
                token->value = op1->value < op2->value;
                token->opCode = OpCodes::LITERAL;
                delete op1;
                delete op2;
            }
        }

        break;
    }

    case OpCodes::LOGICAL_LESS_EQ:
    {
        binarySatisfy(token, TokenType::NUMERIC, TokenType::NUMERIC, statement);

        if (globals::doOptimize)
        {
            Token* op1 = ((Token**) (token->value))[0];
            Token* op2 = ((Token**) (token->value))[1];

            if (op1->opCode == OpCodes::LITERAL && op2->opCode == OpCodes::LITERAL)
            {
                delete[] (Token**) token->value;
                token->value = op1->value <= op2->value;
                token->opCode = OpCodes::LITERAL;
                delete op1;
                delete op2;
            }
        }

        break;
    }

    case OpCodes::LOGICAL_GREATER:
    {
        binarySatisfy(token, TokenType::NUMERIC, TokenType::NUMERIC, statement);

        if (globals::doOptimize)
        {
            Token* op1 = ((Token**) (token->value))[0];
            Token* op2 = ((Token**) (token->value))[1];

            if (op1->opCode == OpCodes::LITERAL && op2->opCode == OpCodes::LITERAL)
            {
                delete[] (Token**) token->value;
                token->value = op1->value > op2->value;
                token->opCode = OpCodes::LITERAL;
                delete op1;
                delete op2;
            }
        }

        break;
    }

    case OpCodes::LOGICAL_GREATER_EQ:
    {
        binarySatisfy(token, TokenType::NUMERIC, TokenType::NUMERIC, statement);

        if (globals::doOptimize)
        {
            Token* op1 = ((Token**) (token->value))[0];
            Token* op2 = ((Token**) (token->value))[1];

            if (op1->opCode == OpCodes::LITERAL && op2->opCode == OpCodes::LITERAL)
            {
                delete[] (Token**) token->value;
                token->value = op1->value >= op2->value;
                token->opCode = OpCodes::LITERAL;
                delete op1;
                delete op2;
            }
        }

        break;
    }
    
    case OpCodes::LOGICAL_NOT:
    {
        unarySatisfy(token, TokenType::BOOL, RIGHT, statement);

        if (globals::doOptimize)
        {
            Token* op = ((Token**) (token->value))[0];

            if (op->opCode == OpCodes::LITERAL)
            {
                delete[] (Token**) token->value;
                token->value = !(bool) op->value;
                token->opCode = OpCodes::LITERAL;
                delete op;
            }
        }

        break;
    }


    case OpCodes::ADDRESS_OF:
    {
        addressOfSatisfy(token, statement);
        break;
    }


    case OpCodes::ASSIGNMENT_ASSIGN:
    case OpCodes::ASSIGNMENT_ADD:
    case OpCodes::ASSIGNMENT_SUB:
    case OpCodes::ASSIGNMENT_DIV:
    case OpCodes::ASSIGNMENT_POW:
    case OpCodes::ASSIGNMENT_MUL:
    {
        assignSatisfy(token, statement);
        break;
    }


    case OpCodes::DECLARATION_INT:
        declarationSatisfy(token, TokenType::INT, statement);
        break;

    case OpCodes::DECLARATION_STRING:
        declarationSatisfy(token, TokenType::STRING, statement);
        break;

    case OpCodes::DECLARATION_FLOAT:
        declarationSatisfy(token, TokenType::FLOAT, statement);
        break;

    case OpCodes::DECLARATION_DOUBLE:
        declarationSatisfy(token, TokenType::DOUBLE, statement);
        break;

    case OpCodes::DECLARATION_LONG:
        declarationSatisfy(token, TokenType::LONG, statement);
        break;

    case OpCodes::DECLARATION_BOOL:
        declarationSatisfy(token, TokenType::BOOL, statement);
        break;
    
    case OpCodes::DECLARATION_BYTE:
        declarationSatisfy(token, TokenType::BYTE, statement);
        break;
    
    case OpCodes::DECLARATION_VOID:
    {
        // TODO check if it's a pointer declaration
        // otherwise throw error since a variable cannot be void
        
        assertToken(token, token->next, OpCodes::ADDRESS_OF, RIGHT);

        break;
    }

    
    case OpCodes::PUSH_SCOPE:
    case OpCodes::FUNC_BODY:
    {
        // push a new scope only if it's a standalone scope
        // function bodies' scopes are pushed and popped by the function
        // declaration operator
        if (token->opCode == OpCodes::PUSH_SCOPE)
        {
            // first push the scope to the SymbolTable
            SymbolTable::pushScope(DO_INHERIT);
        }

        // transfer the part of the statement which is in the new scope
        // to another statement
        Statement* scopeStatement = new Statement(token->next);
        scopeStatement->next = statement->next;
        
        // set previous token to null since this is the beginning of the statement
        scopeStatement->root->prev = nullptr;

        // create a new statement list for the new syntax tree
        Statements scopeStatements = Statements(scopeStatement);        

        // search for closing scope
        unsigned int depth = 1;
        for (Token* tok = scopeStatement->root; true; tok = tok->next)
        {
            // jump to next statement if current statement is finished
            if (tok == nullptr)
            {
                // search for closing scope in next statement
                scopeStatement = scopeStatement->next;

                // check if the statement actually exists
                if (scopeStatement == nullptr)
                {
                    std::cerr << "Missing closing scope because next statement is missing" << std::endl;
                    exit(1);
                }

                tok = scopeStatement->root;
            }

            // increase or decrease scope depth if another scope is found
            if (tok->opCode == OpCodes::PUSH_SCOPE)
            {
                depth ++;
            }
            else if (tok->opCode == OpCodes::POP_SCOPE)
            {
                depth --;

                // if depth is 0 --> found end of scope
                if (depth == 0)
                {
                    // this is the last statement of the scope
                    scopeStatements.end = scopeStatement;

                    // make the statement (the one before the scope) 
                    // continue from this token's next
                    if (tok->next != nullptr)
                    {
                        tok->next->prev = token;
                    }
                    token->next = tok->next;

                    // make the scopeStatement end right before the closing scope Token
                    if (tok->prev != nullptr)
                    {
                        tok->prev->next = nullptr;
                    }

                    // delete closing scope token since it won't be used anymore
                    delete tok;

                    // make the outer Statement linked list (Statements)
                    // continue from the next statement
                    statement->next = scopeStatement->next;

                    // if statement's root is the newly deleted token --> delete it
                    if (scopeStatement->root == tok)
                    {
                        scopeStatements.removeLast();
                        delete scopeStatement;
                    }
                    // whereas if the statement isn't empty
                    else
                    {
                        // break the linked list on end of scope
                        scopeStatement->next = nullptr;
                    }
                    
                    break;
                }
            }
        }
        
        // create a new SyntaxTree for the scope
        SyntaxTree* scopeTree = new SyntaxTree(std::move(scopeStatements));

        scopeTree->parseToByteCodePrivate(); 

        // if token is a standalone scope, pop it function
        // bodies are popped by the function declaration operator
        if (token->opCode == OpCodes::PUSH_SCOPE)
        {     
            SymbolTable::popScope();
        }

        // don't use a Token** since it would be superflous
        token->value = toValue(scopeTree);

        break;
    }
    

    case OpCodes::PARENTHESIS:
    {
        // check if there's anything inside parenthesis
        Token* content = token->next;
        if (content == nullptr)
        {
            std::cerr << "Missing statement inside parenthesis" << std::endl;
            exit(1);
        }

        // check for closing parenthesis
        Token* closing = content->next;
        if (closing == nullptr || 
            (closing->opCode != OpCodes::PARENTHESIS && closing->value != ')'))
        {
            std::cerr << "Missing closing parenthesis" << std::endl;
            exit(1);
        }

        if (globals::doOptimize)
        {
            // transform parenthesis into its content
            copyRelevantData(token, content);

            // to prevent token's destructor from deleting eventual stuff pointed by its value
            content->opCode = OpCodes::NO_OP;

            statement->remove(content, DELETE),
            statement->remove(closing, DELETE);

            break;
        }

        // parenthesis' value is it's content
        token->value = toValue(new Token*[1] { content });

        // set parenthesis' type to it's content's
        token->type = tokenTypeOf(content);

        statement->remove(content);
        statement->remove(closing, DELETE);

        break;
    }

    
    case OpCodes::FLOW_IF:
    {
        Token* condition = token->next;
        assertToken(token, condition, TokenType::BOOL, RIGHT);

        Token* body = condition->next;

        // check if body exists
        if (body == nullptr)
        {
            errors::ExpectedTokenError(*condition, OpCodes::PUSH_SCOPE, sides[RIGHT]);
        } 

        if (globals::doOptimize)
        {
            // remove branch or condition checking if condition's value is known at compile time
            if (
                condition->opCode == OpCodes::LITERAL
                || condition->opCode == OpCodes::REFERENCE
                )
            {
                if (
                    (condition->opCode == OpCodes::LITERAL && condition->value == 0)
                    || (condition->opCode == OpCodes::REFERENCE
                        && SymbolTable::get((std::string*) condition->value)->value == 0)
                    )
                {
                    // remove branch since it's always false
                    statement->remove(condition, DELETE);
                    statement->remove(body, DELETE);
                    statement->remove(token, DELETE);

                    break;
                }
                // if it's true transform token into its body (Token holding a SyntaxTree)
                copyRelevantData(token, body);
                
                // set body's opCode to NO_OP to prevent it from deleting its Syntax Tree 
                // when its destructor is called
                body->opCode = OpCodes::NO_OP;
                statement->remove(body, DELETE);
                statement->remove(condition, DELETE);

                break;
            }
        }

        // set if token's value to an array of its boolean condition and its body
        token->value = toValue((new Token*[2] { condition, body }));

        // remove operands from the statement (this)
        statement->remove(condition);
        statement->remove(body);

        break;
    }


    case OpCodes::FLOW_WHILE:
    {
        Token* condition = token->next;
        assertToken(token, condition, TokenType::BOOL, RIGHT);

        Token* body = condition->next;

        // check if body exists
        if (body == nullptr)
        {
            errors::ExpectedTokenError(*condition, OpCodes::PUSH_SCOPE, sides[RIGHT]);
        } 

        // set if token's value to an array of its boolean condition and its body
        token->value = toValue((new Token*[2] { condition, body }));

        // remove operands from the statement (this)
        statement->remove(condition);
        statement->remove(body);

        break;
    }


    case OpCodes::FLOW_ELSE:
    {
        Token* body = token->next;

        // check if body exists
        if (body == nullptr)
        {
            std::cerr << "Missing else statement's body" << std::endl;
            exit(1);
        }

        // do not add "if" statements as leaves
        if (body->opCode == OpCodes::FLOW_IF)
        {
            break;
        }

        token->value = toValue(new Token*[1] { body });

        statement->remove(body);

        break;
    }


    case OpCodes::FLOW_FOR:
    {
        std::cerr << "not implemented" << std::endl;
        exit(EXIT_FAILURE);
        break;
    }


    case OpCodes::SYSTEM:
    case OpCodes::SYSTEM_LOAD:
    {
        unarySatisfy(token, TokenType::INT, RIGHT, statement);
        break;
    }


    case OpCodes::FUNC_DECLARARION:
    {
        Token* name = token->prev;
        assertToken(token, name, TokenType::TEXT, LEFT);

        Token* returnType = name->prev;
        assertToken(name, returnType, TokenType::KEYWORD, LEFT);
        
        // TODO get eventual modifiers

    // get the function parameters

        auto params = std::vector<Parameter>();

        // push the new scope the parameters will belong to
        SymbolTable::pushScope(DONT_INHERIT);

        // extract parameters
        Token* tok = token->next;
        for (size_t depth = 1; tok != nullptr; )
        {
            // increase depth for every opening parentheies (except for function definition)
            if (tok->opCode == OpCodes::CALL
                || (tok->opCode == OpCodes::PARENTHESIS && tok->value == '('))
            {
                depth ++;
            }
            else if (tok->opCode == OpCodes::PARENTHESIS
                    && tok->value == ')')
            {
                depth --;

                if (depth == 0)
                {
                    // on break variable tok is the closing parenthesis
                    break;
                }
            }

            // declare symbols for parameters and
            // add parameters to the parameter list

            if (isDeclarationOp(tok->opCode))
            {
                // satisfy each parameter before adding to the parameter list
                // here the parameters will be declared in the symbol table
                declarationSatisfy(
                    tok,
                    keywords::declarationType(tok->opCode),
                    statement
                );

                // create a new Parameter object that will represent the just declared
                // parameter in the symbol table.
                // the symbol name's string is moved since it won't be used anymore by the token
                params.emplace_back(
                    Parameter(
                        Symbol(0, tok->type),
                        std::move(*(std::string*) tok->value)
                    )
                );
                
                // to prevent token destructor from deleting string value
                // set the op code to NO_OP (whose destructor does nothing)
                tok->opCode = OpCodes::NO_OP;
                Token* tmpTok = tok->next;

                statement->remove(tok, DELETE);

                tok = tmpTok; 

                continue;
            }

            tok = tok->next;
            
        }
        
        // check if end of statement is reached and a closing parenthesis
        // hasn't been found
        if (tok == nullptr)
        {
            errors::MissingClosingParenthesisError(
                *token,
                std::string("Function named ") + *(std::string*) name->value
                    + " is missing closing parenthesis in function call");
        }

        Token* body = tok->next;
        assertToken(tok, body, OpCodes::PUSH_SCOPE, RIGHT);

        // change the op code of body to a function body operator to 
        // satisfy it properly
        body->opCode = OpCodes::FUNC_BODY;

        // satisfy the function body's token
        // its SyntaxTree value is needed when creating a Function object
        satisfyToken(statement, body);

        // pop the function's body's scope
        SymbolTable::popScope();


        Function* function = new Function(
            keywords::declarationType(returnType->opCode),
            std::move(*(SyntaxTree*) body->value),
            std::move(params)
        );

        // declare the function in the outer (usually global) scope
        SymbolTable::declare(
            (std::string*) name->value,
            new Symbol(toValue(function), TokenType::FUNCTION)
        );

        // delete tokens since they won't be needed anymore
        
        statement->remove(returnType, DELETE);

        // name is a TEXT token (hold a heap-allocated std::string* as value)
        // changing its op code to NO_OP prevents the string to be deleted in the
        // token's destructor
        name->opCode = OpCodes::NO_OP;
        statement->remove(name, DELETE);

        // remove and delete function declaration operator
        statement->remove(token, DELETE);

        // remove and delete closing parenthesis token
        statement->remove(tok, DELETE);

        break;
    }


    } // switch (token->opCode)

}


void SyntaxTree::parseStatement(Statement* statement)
{
    for (
        Tokens::Token* root = getHighestPriority(statement->root);
        // 0 priority or less means the statement has finished evaluating
        root != nullptr && root->priority > 1;
        root = getHighestPriority(statement->root)
        )
    {          

        // evaluate the Token, satisfy its requirements, declare eventual symbols   
        satisfyToken(statement, root);

    }
}

