#include "syntax_tree.hh"
#include "errors.hh"


// for unary operators
typedef enum Side
{
    RIGHT = 0,
    LEFT  = 1
    
} Side;


static const char* sides[2] = 
{
    "right",
    "left"
};


#define DELETE true

using namespace Tokens;
using namespace syntax_tree;


/*
 - returns the TokenType of a given token
 - returns the TokenType of it's value if token is a variable
 - throws exception if token is not declared
*/
static TokenType tokenTypeOf(const Token* token)
{
    if (token->opCode == OpCodes::REFERENCE)
    {
        return symbol_table::SymbolTable::get((std::string*) token->value)->type;
    }
    
    return token->type;
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

    if (tokenTypeOf(token->prev) != leftType)
    {
       errors::TypeError(*token, leftType, *token->prev, sides[LEFT]);
    }
    else if (tokenTypeOf(token->next) != rightType)
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
        if (tokenTypeOf(token->prev) != type)
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
        if (tokenTypeOf(token->next) != type)
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

    // inherit value form next Token
    token->value = token->next->value;
    // type is the TokenType that has been declared
    token->type = type;

    // set next Token's value to nullptr no the std::string value
    // does not get deleted when the Token does
    token->next->value = toValue(nullptr);

    // remove and delete the next Token since it won't be used anymore
    statement->remove(token->next, DELETE);

    token->opCode = OpCodes::REFERENCE;

    symbol_table::SymbolTable::declare(
        (std::string*) token->value,
        new symbol_table::Symbol(0, token->type)
    );

}


static void assignSatisfy(Token* token, Statement* statement)
{

    if (token->prev == nullptr)
    {
        errors::ExpectedTokenError(*token, OpCodes::REFERENCE, sides[LEFT]);
    }    
    else if (token->next == nullptr)
    {
        errors::ExpectedTokenError(*token, OpCodes::REFERENCE, sides[RIGHT]);
    }

    if (token->prev->opCode != OpCodes::REFERENCE)
    {
        errors::TypeError(*token, OpCodes::REFERENCE, *token->prev, sides[LEFT]);
    }
    else if (tokenTypeOf(token->next) != tokenTypeOf(token->prev))
    {
        errors::TypeError(*token, OpCodes::REFERENCE, *token->next, sides[RIGHT]);
    }

    // set token's value to an array of its operands
    token->value = toValue((new Token*[2] {token->prev, token->next}));

    // update symbol table
    symbol_table::SymbolTable::assign(
        (std::string*) token->prev->value,
        new symbol_table::Symbol(token->next->value, token->prev->type)
    );

    // type of assignment operator is the type of the variable it has assigned a value to
    token->type = tokenTypeOf(token->prev);

    // remove tokens to the left and right
    statement->remove(token->prev);
    statement->remove(token->next);

}


// increment ++, decrement -- operators
static void incDecSatisfy(Token* token, Statement* statement)
{
    if (token->prev == nullptr)
    {
        errors::ExpectedTokenError(*token, OpCodes::REFERENCE, sides[LEFT]);
    }
    if (token->prev->opCode != OpCodes::REFERENCE)
    {
        errors::TypeError(*token, OpCodes::REFERENCE, *token->prev, sides[LEFT]);
    }


    token->value = toValue(new Token*[1] { token->prev });
    token->type = tokenTypeOf(token->prev);

    statement->remove(token->prev);

}


static void addressOfSatisfy(Token* token, Statement* statement)
{
    if (token->next == nullptr)
    {
        errors::ExpectedTokenError(*token, OpCodes::REFERENCE, sides[RIGHT]);
    }
    if (token->next->opCode != OpCodes::REFERENCE)
    {
        errors::TypeError(*token, OpCodes::REFERENCE, *token->next, sides[RIGHT]);
    } 
    
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
    case OpCodes::ARITHMETICAL_SUB:
    case OpCodes::ARITHMETICAL_MUL:
    case OpCodes::ARITHMETICAL_DIV:
    case OpCodes::ARITHMETICAL_POW:
    {
        binarySatisfy(token, TokenType::INT, TokenType::INT, statement);
        break;
    }

    case OpCodes::ARITHMETICAL_INC:
    case OpCodes::ARITHMETICAL_DEC:
    {
        incDecSatisfy(token, statement);
        break;
    }


    case OpCodes::LOGICAL_EQ:
    case OpCodes::LOGICAL_NOT_EQ:
    case OpCodes::LOGICAL_AND:
    case OpCodes::LOGICAL_OR:
    case OpCodes::LOGICAL_LESS:
    case OpCodes::LOGICAL_LESS_EQ:
    case OpCodes::LOGICAL_GREATER:
    case OpCodes::LOGICAL_GREATER_EQ:
    {
        binarySatisfy(token, TokenType::BOOL, TokenType::BOOL, statement);
        break;
    }
    
    case OpCodes::LOGICAL_NOT:
    {
        unarySatisfy(token, TokenType::BOOL, RIGHT, statement);
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
    case OpCodes::DECLARATION_BOOL:
        declarationSatisfy(token, TokenType::BOOL, statement);
        break;

    
    case OpCodes::PUSH_SCOPE:
    {
        // first push the scope to the SymbolTable
        symbol_table::SymbolTable::pushScope(DO_INHERIT);

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
        
        // create a temporary SyntaxTree for the scope and move it to the SyntaxTree
        SyntaxTree scopeTree = SyntaxTree(std::move(scopeStatements), doOptimize);

        // parse the new SyntaxTree recursively
        //scopeTree.parse(DO_POP_SCOPE);


        // pop the Scope once parsed its SyntaxTree
        symbol_table::SymbolTable::popScope();

        // set the token's value to the newly generated tac
        //token->value = toValue(scopeTac);

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

        // parenthesis' value is it's content's
        token->value = toValue(new Token*[1] { content });

        // set parenthesis' type to it's content's
        token->type = tokenTypeOf(content);

        statement->remove(content);
        statement->remove(closing, DELETE);

        break;
    }

    
    case OpCodes::FLOW_IF:
    case OpCodes::FLOW_WHILE:
    {
        Token* condition = token->next;

        // check if a boolean condition is provided
        if (condition == nullptr)
        {
            std::cerr << "Missing boolean condidion after if keyword" << std::endl;
            exit(1);
        }

        // check if condition is actually a boolean statement
        if (condition->type != TokenType::BOOL)
        {
            std::cerr << "Expected token of type " << TokenType::BOOL << " after " << token->opCode << " keyword, but " << condition << " was provided" << std::endl;
            exit(EXIT_FAILURE);
        }

        Token* body = condition->next;

        // check if body exists
        if (body == nullptr)
        {
            std::cerr << "Missing " << token->opCode << " statement's body after condition" << std::endl;
            exit(EXIT_FAILURE);
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


    } // switch (token->opCode)

}


void SyntaxTree::parseStatement(Statement* statement)
{
    Tokens::Token* root = statement->root;

    // parse statement
    while (true)
    {   
        
        // return to the beginning of the statement
        while (root->prev != nullptr)
        {
            root = root->prev;
        }

        root = getHighestPriority(root);

        if (root->priority < 1) // 0 or less
        {
            break;
        }

        // evaluate the Token    
        satisfyToken(statement, root);

    }
}

